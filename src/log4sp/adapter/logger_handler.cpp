#include <cassert>
#include <string>

#include "spdlog/spdlog.h"  // SPDLOG_TRACE 和 SPDLOG_DEBUG 需要

#include "log4sp/proxy/logger_proxy.h"

#include "log4sp/adapter/logger_handler.h"


namespace log4sp {

logger_handler &logger_handler::instance() {
    static logger_handler instance;
    return instance;
}

void logger_handler::initialize() {
    instance().initialize_();
}

void logger_handler::destroy() {
    instance().destroy_();
}


HandleType_t logger_handler::handle_type() const {
    return handle_type_;
}

Handle_t logger_handler::create_handle(std::shared_ptr<logger_proxy> object, const HandleSecurity *security, const HandleAccess *access, HandleError *error) {
    Handle_t handle = handlesys->CreateHandleEx(handle_type_, object.get(), security, access, error);
    if (handle == BAD_HANDLE) {
        return BAD_HANDLE;
    }

    assert(handles_.find(object->name()) == handles_.end());
    assert(loggers_.find(object->name()) == loggers_.end());

    handles_[object->name()] = handle;
    loggers_[object->name()] = object;

    SPDLOG_TRACE("A logger handle created. (name: {}, hdl: {})", object->name(), handle);
    return handle;
}

std::shared_ptr<logger_proxy> logger_handler::read_handle(Handle_t handle, HandleSecurity *security, HandleError *error) {
    logger_proxy *object;
    HandleError err = handlesys->ReadHandle(handle, handle_type_, security, (void **)&object);

    if (err != HandleError_None) {
        if (error) {
            *error = static_cast<HandleError>(err);
        }
        return nullptr;
    }

    assert(loggers_.find(object->name()) != loggers_.end());

    auto found = loggers_.find(object->name());
    return found->second;
}

Handle_t logger_handler::find_handle(const std::string &name) {
    auto found = handles_.find(name);
    return found == handles_.end() ? BAD_HANDLE : found->second;
}

std::shared_ptr<logger_proxy> logger_handler::find_logger(const std::string &name) {
    auto found = loggers_.find(name);
    return found == loggers_.end() ? BAD_HANDLE : found->second;
}

std::vector<std::string> logger_handler::get_all_logger_names() {
    std::vector<std::string> names;
    for (const auto &pair : loggers_) {
        names.push_back(pair.first);
    }
    return names;
}

void logger_handler::apply_all(const std::function<void(const Handle_t)> &fun) {
    for (auto &h : handles_) {
        fun(h.second);
    }
}

void logger_handler::apply_all(const std::function<void(std::shared_ptr<logger_proxy>)> &fun) {
    for (auto &l : loggers_) {
        fun(l.second);
    }
}

void logger_handler::OnHandleDestroy(HandleType_t type, void *object) {
    auto logger = static_cast<log4sp::logger_proxy *>(object);

    SPDLOG_TRACE("A logger handle destroyed. (name: {})", logger->name());

    assert(handles_.find(logger->name()) != handles_.end());
    assert(loggers_.find(logger->name()) != loggers_.end());

    handles_.erase(logger->name());
    loggers_.erase(logger->name());
}


logger_handler::~logger_handler() {
    assert(handle_type_ == NO_HANDLE_TYPE);
}

void logger_handler::initialize_() {
    HandleAccess access;
    HandleError error;

    // 默认情况下，创建的 handle 可以被任意插件释放
    handlesys->InitAccessDefaults(nullptr, &access);
    access.access[HandleAccess_Delete] = 0;

    handle_type_ = handlesys->CreateType("Logger", this, 0, nullptr, &access, myself->GetIdentity(), &error);
    if (handle_type_ == NO_HANDLE_TYPE) {
        throw std::runtime_error("Failed to create Logger handle type. (error: " + std::to_string(static_cast<int>(error)) + ")");
    }
}

void logger_handler::destroy_() {
    assert(handle_type_ != NO_HANDLE_TYPE);

    if (handle_type_ != NO_HANDLE_TYPE) {
        handlesys->RemoveType(handle_type_, myself->GetIdentity());
        handle_type_ = NO_HANDLE_TYPE;
    }
}


}       // namespace log4sp

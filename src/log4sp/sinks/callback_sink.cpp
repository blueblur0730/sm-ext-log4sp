#include "log4sp/sinks/callback_sink.h"


namespace log4sp {
namespace sinks {

callback_sink::callback_sink(IPluginFunction *log_function,
                             IPluginFunction *log_post_function,
                             IPluginFunction *flush_function) {
    set_log_callback(log_function);
    try {
        set_log_post_callback(log_post_function);
        set_flush_callback(flush_function);
    } catch(...) {
        release_forwards_();
        throw;
    }
}

callback_sink::~callback_sink() {
    release_forwards_();
}

void callback_sink::release_forwards_() {
    if (log_callback_) {
        forwards->ReleaseForward(log_callback_);
        log_callback_ = nullptr;
    }

    if (log_post_callback_) {
        forwards->ReleaseForward(log_post_callback_);
        log_post_callback_ = nullptr;
    }

    if (flush_callback_) {
        forwards->ReleaseForward(flush_callback_);
        flush_callback_ = nullptr;
    }
}

void callback_sink::set_log_callback(IPluginFunction *log_function) {
    IChangeableForward *cb{nullptr};

    if (log_function) {
        cb = forwards->CreateForwardEx(nullptr, ET_Ignore, 8, nullptr,
                                       Param_String, Param_Cell, Param_String,
                                       Param_String, Param_Cell, Param_String,
                                       Param_Array, Param_Array);
        if (!cb) {
            throw std::runtime_error{"SM error! Could not create callback sink log forward."};
        }

        if (!cb->AddFunction(log_function)) {
            forwards->ReleaseForward(cb);
            throw std::runtime_error{"SM error! Could not add callback sink log function."};
        }
    }

    if (log_callback_) {
        forwards->ReleaseForward(log_callback_);
    }
    log_callback_ = cb;
}

void callback_sink::set_log_post_callback(IPluginFunction *log_post_function) {
    IChangeableForward *cb{nullptr};

    if (log_post_function) {
        cb = forwards->CreateForwardEx(nullptr, ET_Ignore, 1, nullptr, Param_String);
        if (!cb) {
            throw std::runtime_error{"SM error! Could not create callback sink log post forward."};
        }

        if (!cb->AddFunction(log_post_function)) {
            forwards->ReleaseForward(cb);
            throw std::runtime_error{"SM error! Could not add callback sink log post function."};
        }
    }

    if (log_post_callback_) {
        forwards->ReleaseForward(log_post_callback_);
    }
    log_post_callback_ = cb;
}

void callback_sink::set_flush_callback(IPluginFunction *flush_function) {
    IChangeableForward *cb{nullptr};

    if (flush_function) {
        cb = forwards->CreateForwardEx(nullptr, ET_Ignore, 0, nullptr);
        if (!cb) {
            throw std::runtime_error{"SM error! Could not create callback sink flush forward."};
        }

        if (!cb->AddFunction(flush_function)) {
            forwards->ReleaseForward(cb);
            throw std::runtime_error{"SM error! Could not add callback sink flush function."};
        }
    }

    if (flush_callback_) {
        forwards->ReleaseForward(flush_callback_);
    }
    flush_callback_ = cb;
}

void callback_sink::sink_it_(const spdlog::details::log_msg &msg) {
    if (log_callback_) {
        int64_t sec = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::seconds>(msg.time.time_since_epoch()).count());
        int64_t ns  = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(msg.time.time_since_epoch()).count());

        // See SMCore::GetTime
        cell_t pSec[]{static_cast<cell_t>(sec & 0xFFFFFFFF), static_cast<cell_t>((sec >> 32) & 0xFFFFFFFF)};
        cell_t pNs[]{static_cast<cell_t>(ns & 0xFFFFFFFF), static_cast<cell_t>((ns >> 32) & 0xFFFFFFFF)};

        log_callback_->PushString(msg.logger_name.data());
        log_callback_->PushCell(msg.level);
        log_callback_->PushString(msg.payload.data());

        log_callback_->PushString(msg.source.filename);
        log_callback_->PushCell(msg.source.line);
        log_callback_->PushString(msg.source.funcname);

        log_callback_->PushArray(pSec, sizeof(pSec));
        log_callback_->PushArray(pNs, sizeof(pNs));
        log_callback_->Execute();
    }

    if (log_post_callback_) {
        std::string formatted = to_pattern(msg);
        log_post_callback_->PushString(formatted.c_str());
        log_post_callback_->Execute();
    }
}

void callback_sink::flush_() {
    if (flush_callback_) {
        flush_callback_->Execute();
    }
}


}       // namespace sinks
}       // namespace log4sp

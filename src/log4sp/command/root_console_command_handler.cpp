#include <cassert>
#include <exception>

#include "spdlog/common.h"
#include "spdlog/fmt/xchar.h"

#include "log4sp/command/root_console_command_handler.h"

namespace log4sp {

root_console_command_handler &root_console_command_handler::instance() {
    static root_console_command_handler singleInstance;
    return singleInstance;
}

void root_console_command_handler::initialize() {
    instance().initialize_();
}

void root_console_command_handler::destroy() {
    instance().destroy_();
}


void root_console_command_handler::draw_menu() {
    rootconsole->ConsolePrint("Logging for SourcePawn Menu:");
    rootconsole->ConsolePrint("Usage: sm log4sp <function_name> [arguments]");

    rootconsole->DrawGenericOption("list",          "List all logger names.");
    rootconsole->DrawGenericOption("apply_all",     "Apply a command function on all loggers.");
    rootconsole->DrawGenericOption("get_lvl",       spdlog::fmt_lib::format("Gets a logger log level. [{}]", spdlog::fmt_lib::join(spdlog::level::level_string_views, " < ")).c_str());
    rootconsole->DrawGenericOption("set_lvl",       spdlog::fmt_lib::format("Sets a logger log level. [{}]", spdlog::fmt_lib::join(spdlog::level::level_string_views, " < ")).c_str());
    rootconsole->DrawGenericOption("set_pattern",   "Sets a logger log pattern.");
    rootconsole->DrawGenericOption("should_log",    "Gets a logger whether logging is enabled for the given log level.");
    rootconsole->DrawGenericOption("log",           "Use a logger to log a message.");
    rootconsole->DrawGenericOption("flush",         "Manual flush a logger contents.");
    rootconsole->DrawGenericOption("get_flush_lvl", "Gets the minimum log level that will trigger automatic flush.");
    rootconsole->DrawGenericOption("set_flush_lvl", "Sets the minimum log level that will trigger automatic flush.");
    rootconsole->DrawGenericOption("should_bt",     "Gets a logger whether backtrace logging is enabled.");
    rootconsole->DrawGenericOption("enable_bt",     "Enable a logger backtrace logging.");
    rootconsole->DrawGenericOption("disable_bt",    "Disaable a logger backtrace logging.");
    rootconsole->DrawGenericOption("dump_bt",       "Dump a logger backtrace logging messages.");
}


void root_console_command_handler::execute(const std::string &cmdname, const std::vector<std::string> &args) {
    auto iter = commands_.find(cmdname);
    if (iter != commands_.end()) {
        iter->second->execute(args);
    } else {
        throw std::runtime_error(spdlog::fmt_lib::format("The function name '{}' does not exist.", cmdname));
    }
}


void root_console_command_handler::OnRootConsoleCommand(const char *cmdname, const ICommandArgs *args) {
    // 0-sm  |  1-log4sp  |  2-function name  |  3-logger name  |  x-params
    int argCnt = args->ArgC();
    if (argCnt <= 2) {
        draw_menu();
        return;
    }

    std::string function_name {args->Arg(2)};

    std::vector<std::string> arguments;
    for (int i = 3; i < argCnt; ++i) {
        arguments.push_back(args->Arg(i));
    }

    try {
        execute(function_name, arguments);
    } catch (const std::exception &ex) {
        rootconsole->ConsolePrint("[SM] %s", ex.what());
    }
}

root_console_command_handler::root_console_command_handler() {
    commands_["list"]           = std::make_unique<list_command>();
    commands_["apply_all"]      = std::make_unique<apply_all_command>(std::unordered_set<std::string>{"get_lvl", "set_lvl", "set_pattern", "should_log", "log", "flush", "get_flush_lvl", "set_flush_lvl", "should_bt", "enable_bt", "disable_bt", "dump_bt"});
    commands_["get_lvl"]        = std::make_unique<get_lvl_command>();
    commands_["set_lvl"]        = std::make_unique<set_lvl_command>();
    commands_["set_pattern"]    = std::make_unique<set_pattern_command>();
    commands_["should_log"]     = std::make_unique<should_log_command>();
    commands_["log"]            = std::make_unique<log_command>();
    commands_["flush"]          = std::make_unique<flush_command>();
    commands_["get_flush_lvl"]  = std::make_unique<get_flush_lvl_command>();
    commands_["set_flush_lvl"]  = std::make_unique<set_flush_lvl_command>();
    commands_["should_bt"]      = std::make_unique<should_bt_command>();
    commands_["enable_bt"]      = std::make_unique<enable_bt_command>();
    commands_["disable_bt"]     = std::make_unique<disable_bt_command>();
    commands_["dump_bt"]        = std::make_unique<dump_bt_command>();
}

void root_console_command_handler::initialize_() {
    if (!rootconsole->AddRootConsoleCommand3(SMEXT_CONF_LOGTAG, "Logging for SourcePawn command menu", this)) {
        throw std::runtime_error("Failed to add root console commmand \"" + std::string{SMEXT_CONF_LOGTAG} + "\".");
    }
}

void root_console_command_handler::destroy_() {
    bool result = rootconsole->RemoveRootConsoleCommand(SMEXT_CONF_LOGTAG, this);
    assert(result);
}


}       // namespace log4sp
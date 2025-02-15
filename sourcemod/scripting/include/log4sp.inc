#if defined _log_for_sourcepawn_included
 #endinput
#endif
#define _log_for_sourcepawn_included

#pragma newdecls required
#pragma semicolon 1



/**
 * Define the LOG4SP_NO_EXT macro before including this header file to make the
 * plugin not depend on the log4sp extension.
 *
 * This will replace most of the log4sp natives to empty statements or do nothing,
 * except for the following:
 *
 * - Logger.GetLevel()
 *      Always returns LogLevel_Info.
 *
 * - Logger.ShouldLog()
 *      Returns true when param lvl is equal to or greater than LogLevel_Info.
 *
 * - Logger.Log(), Logger.LogAmxTpl(),
 *   Logger.LogSrc(), Logger.LogSrcAmxTpl(),
 *   Logger.LogLoc(), Logger.LogLocAmxTpl()
 *      If param lvl is equal to INFO or WARN, simply use LogMessage() instead;
 *      If param lvl is equal to ERROR or FATAL, simply use LogError() instead;
 *      else do nothing.
 *
 * - Logger.LogStackTrace(), Logger.LogStackTraceAmxTpl()
 *      Simply use LogStackTrace() instead.
 *
 * - Logger.ThrowError(), Logger.ThrowErrorAmxTpl()
 *      Simply use ThrowError() instead.
 *
 * - Logger.Info(), Logger.InfoAmxTpl(),
 * - Logger.Warn(), Logger.WarnAmxTpl()
 *      Simply use LogMessage() instead.
 *
 * - Logger.Error(), Logger.ErrorAmxTpl(),
 * - Logger.Fatal(), Logger.FatalAmxTpl()
 *      Simply use LogError() instead.
 */
// #define LOG4SP_NO_EXT



#if !defined LOG4SP_NO_EXT

#include <log4sp/common>
#include <log4sp/logger>
#include <log4sp/sinks/sink>
#include <log4sp/sinks/base_file_sink>
#include <log4sp/sinks/callback_sink>
#include <log4sp/sinks/client_chat_all_sink>
#include <log4sp/sinks/client_console_all_sink>
#include <log4sp/sinks/daily_file_sink>
#include <log4sp/sinks/ringbuffer_sink>
#include <log4sp/sinks/rotating_file_sink>
#include <log4sp/sinks/server_console_sink>

#else

#include <log4sp_no_ext/common>
#include <log4sp_no_ext/logger>
#include <log4sp_no_ext/sinks/sink>
#include <log4sp_no_ext/sinks/base_file_sink>
#include <log4sp_no_ext/sinks/callback_sink>
#include <log4sp_no_ext/sinks/client_chat_all_sink>
#include <log4sp_no_ext/sinks/client_console_all_sink>
#include <log4sp_no_ext/sinks/daily_file_sink>
#include <log4sp_no_ext/sinks/ringbuffer_sink>
#include <log4sp_no_ext/sinks/rotating_file_sink>
#include <log4sp_no_ext/sinks/server_console_sink>

#endif








/**
 * Do not edit below this line!
 */
#define LOG4SP_EXT_VERSION              "1.7.0"

public Extension __ext_log4sp =
{
    name = "Log for SourcePawn",
    file = "log4sp.ext",
#if defined AUTOLOAD_EXTENSIONS && !defined LOG4SP_NO_EXT
    autoload = 1,
#else
    autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS && !defined LOG4SP_NO_EXT
    required = 1,
#else
    required = 0,
#endif
};

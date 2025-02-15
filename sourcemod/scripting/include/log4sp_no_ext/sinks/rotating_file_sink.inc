#if defined _log_for_sourcepawn_sinks_rotating_file_sink_included
 #endinput
#endif
#define _log_for_sourcepawn_sinks_rotating_file_sink_included

#pragma newdecls required
#pragma semicolon 1

#include <log4sp_no_ext/sinks/sink>


methodmap RotatingFileSink < Sink
{
    public RotatingFileSink(const char[] file, const int maxFileSize, const int maxFiles, bool rotateOnOpen = false) {
        return view_as<RotatingFileSink>(INVALID_HANDLE);
    }

    public int GetFilename(char[] buffer, int maxlength) {}

    public void RotateNow() {}

    public static int CalcFilename(const char[] file, int index, char[] buffer, int maxlength) {}

    public static Logger CreateLogger(const char[] name, const char[] file, int maxFileSize, int maxFiles, bool rotateOnOpen = false) {
        return null;
    }
}

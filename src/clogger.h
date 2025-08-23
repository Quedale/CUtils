#ifndef C_LOGGER_H_ 
#define C_LOGGER_H_

#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
 #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
 #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif /* defined(_WIN32) || defined(_WIN64) */

#ifdef ANDROID
#include <android/log.h>
#endif

typedef enum {
#ifdef ANDROID
    C_OFF_E = ANDROID_LOG_SILENT,
    C_FATAL_E = ANDROID_LOG_FATAL,
    C_ERROR_E = ANDROID_LOG_ERROR,
    C_WARN_E = ANDROID_LOG_WARN,
    C_INFO_E = ANDROID_LOG_INFO,
    C_FIXME_E = ANDROID_LOG_WARN, //Fixme doesn't exists on android
    C_DEBUG_E = ANDROID_LOG_DEBUG,
    C_TRACE_E = ANDROID_LOG_VERBOSE,
    C_TRAIL_E = ANDROID_LOG_DEFAULT,
    C_ALL_E = ANDROID_LOG_UNKNOWN
#else
    C_OFF_E = -1,
    C_FATAL_E = 0,
    C_ERROR_E = 1,
    C_WARN_E = 2,
    C_INFO_E = 3,
    C_FIXME_E = 4,
    C_DEBUG_E = 5,
    C_TRACE_E = 6,
    C_TRAIL_E = 7,
    C_ALL_E = 8
#endif
} CLevel;

#ifdef ANDROID
#define C_LOG(level, fmt, ...)  c_log(level, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define C_LOG(level, fmt, ...)  c_log(level, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
void c_log_set_thread_color(char * ansi_color, long threadid);
#endif

#define C_TRAIL(fmt, ...) C_LOG(C_TRAIL_E, fmt, ##__VA_ARGS__)
#define C_TRACE(fmt, ...) C_LOG(C_TRACE_E, fmt, ##__VA_ARGS__)
#define C_DEBUG(fmt, ...) C_LOG(C_DEBUG_E, fmt, ##__VA_ARGS__)
#define C_INFO(fmt, ...)  C_LOG(C_INFO_E , fmt, ##__VA_ARGS__)
#define C_FIXME(fmt, ...)  C_LOG(C_FIXME_E , fmt, ##__VA_ARGS__)
#define C_WARN(fmt, ...)  C_LOG(C_WARN_E , fmt, ##__VA_ARGS__)
#define C_ERROR(fmt, ...) C_LOG(C_ERROR_E, fmt, ##__VA_ARGS__)
#define C_FATAL(fmt, ...) C_LOG(C_FATAL_E, fmt, ##__VA_ARGS__)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_HL_RED  "\033[48:2:255:165:0m"
#define ANSI_COLOR_DRK_GREEN "\x1b[38;5;2m"
#define ANSI_COLOR_HL_GREEN "\x1b[48;5;46m\x1b[38;5;0m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void c_log_set_level(CLevel level);
CLevel c_log_get_level();
void c_log(CLevel level, const char* file, int line, const char* fmt, ...);

#endif

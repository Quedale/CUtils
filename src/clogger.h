#ifndef C_LOGGER_H_ 
#define C_LOGGER_H_

#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
 #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
 #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif /* defined(_WIN32) || defined(_WIN64) */

#define C_TRACE(fmt, ...) c_log(C_TRACE_E, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define C_DEBUG(fmt, ...) c_log(C_DEBUG_E, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define C_INFO(fmt, ...)  c_log(C_INFO_E , __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define C_WARN(fmt, ...)  c_log(C_WARN_E , __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define C_ERROR(fmt, ...) c_log(C_ERROR_E, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define C_FATAL(fmt, ...) c_log(C_FATAL_E, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_HL_RED  "\033[48:2:255:165:0m"
#define ANSI_COLOR_DRK_GREEN "\x1b[38;5;2m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum {
    C_OFF_E = -1,
    C_FATAL_E = 0,
    C_ERROR_E = 1,
    C_WARN_E = 2,
    C_INFO_E = 3,
    C_DEBUG_E = 4,
    C_TRACE_E = 5,
    C_ALL_E = 6
} CLevel;

void c_log_set_thread_color(char * ansi_color, long threadid);
void c_log_set_level(CLevel level);
void c_log(CLevel level, const char* file, int line, const char* fmt, ...);

#endif
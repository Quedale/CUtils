#include "clogger.h"
#include "portable_thread.h"

#include <stdarg.h>

#if defined(_WIN32) || defined(_WIN64)
#else
#   include <sys/time.h>
#endif 

#include <stdio.h>

const char * UNKNOWN_C_LEVEL = "UNKN ";
const char * C_LEVEL_STRING[] = {
    "FATAL",
    "ERROR",
    "WARN ",
    "INFO ",
    "DEBUG",
    "TRACE"
};

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_HL_RED  "\033[48:2:255:165:0m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG_PREFIX_FMT "%s %s %ld %s:%d: "
#define LOG_TIME_FMT "%y-%m-%d %H:%M:%S"
#define LOG_UTIME_FMT ".%06ld"
#define LOG_NEWLINE_STR "\n"
#define LOG_NEWLINE_CHAR '\n'
#define LOG_NULL_STR "(null)"

const char * C_LEVEL_COLOR[] = {
    ANSI_COLOR_HL_RED, //Fatal = red
    ANSI_COLOR_RED, //Error = red
    ANSI_COLOR_YELLOW, //Warning = yellow
    ANSI_COLOR_BLUE, //Info = blue
    ANSI_COLOR_CYAN, //Debug = cyan
    ANSI_COLOR_RESET //Trace = reset
};

static P_MUTEX_TYPE logger_lock = P_MUTEX_INITIALIZER;

//Global properties protected by lock
static CLevel C_LEVEL_SET = C_TRACE_E;
static int wait_fmt_val = 0; //fmt indicator that % was crossed
static char fmt_buff[30]; //fmt buff while parsing arguments
static struct tm calendar; //Calendar pointer used to convert number to string
static char strlvl[27];
static char strtime[32];
static long threadID;
static va_list carg;
static long data_written = 0;

void c_log_set_level(CLevel level){
    C_LEVEL_SET = level;
}

static const char * c_level_to_string(CLevel level){
    if(level < C_ALL_E){
        return C_LEVEL_STRING[level];
    } else {
        return UNKNOWN_C_LEVEL;
    }
}

static const char * c_level_get_color(CLevel level){
    if(level < C_ALL_E){
        return C_LEVEL_COLOR[level];
    } else {
        return ANSI_COLOR_RESET;
    }
}

static void c_level_with_color(CLevel lvl, char * output){
    const char * strlvl = c_level_to_string(lvl);
    const char * strcolor = c_level_get_color(lvl);
    sprintf(output, "%s%s%s", strcolor,strlvl,ANSI_COLOR_RESET);
}

static void time_to_string(const struct timeval* time, char* timestamp, size_t size){
    if(size < 25){
        printf("Logger error, invalid time size...\n");
        return;
    }

    localtime_r(&(time->tv_sec), &calendar);
    strftime(timestamp, size, LOG_TIME_FMT, &calendar);
    sprintf(&timestamp[17], LOG_UTIME_FMT, (long) time->tv_usec);
}

static int is_specifier(char * c){
    if(c[0] == 'c' ||
        c[0] == 'd' || c[0] == 'i' ||
        c[0] == 'e' ||
        c[0] == 'E' ||
        c[0] == 'f' ||
        c[0] == 'g' ||
        c[0] == 'G' ||
        c[0] == 'o' ||
        c[0] == 's' ||
        c[0] == 'u' ||
        c[0] == 'x' ||
        c[0] == 'X' ||
        c[0] == 'p' ||
        c[0] == 'n' ||
        c[0] == '%'){
        return 1;
    }
    return 0;
}

static void c_priv_log_string(FILE* fp, char * strlvl, const char* timestamp, 
        long threadID, const char* file, int line, va_list arg, int * size, long * totalsize){
    const char * str_arg = va_arg(arg,const char *);

    if(!str_arg){
        if ((*size = fputs(LOG_NULL_STR,fp)) > 0) *totalsize += *size;
        return;
    }

    for(int i=0;i<strlen(str_arg);i++){
        //Print character
        if ((*size = fputc(str_arg[i],fp)) > 0) *totalsize += *size;

        //Print line prefix if newline character was added
        if(str_arg[i] == LOG_NEWLINE_CHAR && (*size = fprintf(fp, LOG_PREFIX_FMT, strlvl, timestamp, threadID, file, line)) > 0) *totalsize += *size;
    }
}

static void c_priv_log_line(FILE* fp, char * strlvl, const char* timestamp, 
        long threadID, const char* file, int line, char* log_line, va_list arg, int * size, long * totalsize){

    for(int i=0;i<=strlen(log_line);i++){
        if(wait_fmt_val){
            fmt_buff[wait_fmt_val]=log_line[i];
            if(is_specifier(&(log_line[i]))){
                fmt_buff[wait_fmt_val+1]='\0';//Null terminated string
                if(log_line[i] == 's'){ //s has the potential of having newlines
                    c_priv_log_string(fp,strlvl, timestamp, threadID, file, line, arg, size, totalsize);
                } else {
                    if ((*size = vfprintf(fp,fmt_buff, arg)) > 0) *totalsize += *size;
                }
                wait_fmt_val=0;
            } else {
                wait_fmt_val++;
            }
            continue;
        }

        if(log_line[i] == '%' && i != strlen(log_line)){
            wait_fmt_val=1;
            fmt_buff[0] = log_line[i];
            continue;
        }

        if ((*size = fputc(log_line[i],fp)) > 0) *totalsize += *size;
    }
}

static char* LOG_TOK_PTR;
static int LOG_TMP_SIZE;
static void c_priv_log(FILE* fp, char * strlvl, const char* timestamp, 
        long threadID, const char* file, int line, const char* fmt, va_list arg, long * totalsize){

    LOG_TMP_SIZE = 0;

    //Properly drop const
    char cfmt[strlen(fmt)+1];
    strcpy(cfmt,fmt);

    //Break down log by line to insert prefix
    char * tok = strtok_r(cfmt, LOG_NEWLINE_STR, &LOG_TOK_PTR);
    while (tok) {
        //Print line prefix
        if ((LOG_TMP_SIZE = fprintf(fp, LOG_PREFIX_FMT, strlvl, timestamp, threadID, file, line)) > 0) *totalsize += LOG_TMP_SIZE;

        //Print actual line
        c_priv_log_line(fp, strlvl, timestamp, threadID, file, line, tok, arg, &LOG_TMP_SIZE, totalsize);

        //Print newline
        if ((LOG_TMP_SIZE = fprintf(fp, LOG_NEWLINE_STR)) > 0) *totalsize += LOG_TMP_SIZE;

        tok = strtok_r(NULL, LOG_NEWLINE_STR, &LOG_TOK_PTR);
    }
}

void c_log(CLevel level, const char* file, int line, const char* fmt, ...){
    //Defining now locally so that it grabs the time before the lock
    struct timeval now;
    
    // c_log_set_level(C_WARN_E);
    if(level > C_LEVEL_SET) return;
    
    //Get level string constant
    c_level_with_color(level,strlvl);

    //Get current time of entry
    gettimeofday(&now, NULL);

    va_start(carg, fmt);
    P_MUTEX_LOCK(logger_lock);
    flockfile(stdout);

    //Convert time ulong to char*
    time_to_string(&now, strtime, sizeof(strtime));

    //Get current threadid
    threadID = P_THREAD_ID;
    
    //TODO Support file rotate
    c_priv_log(stdout , strlvl, strtime, threadID, file, line, fmt, carg, &data_written);

    funlockfile(stdout);
    P_MUTEX_UNLOCK(logger_lock);
    va_end(carg);
}
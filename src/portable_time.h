#ifndef PORTABLE_TIME_H
#define PORTABLE_TIME_H

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#   include <time.h>
#define localtime_p(x,y) const time_t timet = x; localtime_s(y,&timet);
int gettimeofday(struct timeval * tp, struct timezone * tzp);

#else
#   include <sys/time.h>
#define localtime_p(x,y) localtime_r(&(x), y);

#endif

#endif
#ifndef PORTABLE_PRINT_H
#define PORTABLE_PRINT_H

#include <stdio.h>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#   define P_LOCK_FILE(x) OVERLAPPED overlapvar = { 0 }; LockFileEx(x, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, MAXDWORD, MAXDWORD, &overlapvar)
#   define P_UNLOCK_FILE(x) UnlockFileEx(x, 0, MAXDWORD, 0, &overlapvar);
#   define strtok_r strtok_s
#else
#   define P_LOCK_FILE(x) flockfile(x)
#   define P_UNLOCK_FILE(x) funlockfile(x)
#endif


#endif
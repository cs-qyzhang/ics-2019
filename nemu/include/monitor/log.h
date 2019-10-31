/* Log 信息有关定义 */
#ifndef __LOG_H__
#define __LOG_H__

#include "common.h"

#ifdef DEBUG
extern FILE* log_fp;
#	define log_write(...) \
  do { \
    if (log_fp != NULL) { \
      fprintf(log_fp, __VA_ARGS__); \
      fflush(log_fp); \
    } \
  } while (0)
#else
#	define log_write(...)
#endif // DEBUG

#define _Log(...) \
  do { \
    printf(__VA_ARGS__); \
    log_write(__VA_ARGS__); \
  } while (0)

void strcatf(char *buf, const char *fmt, ...);

#endif // __LOG_H__

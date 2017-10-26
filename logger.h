#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>

#define LOGGER_FMT_BOLD      "\33[1m"
#define LOGGER_FMT_UNDERLINE "\33[4m"
#define LOGGER_FMT_GREEN     "\33[1;32m"
#define LOGGER_FMT_YELLOW    "\33[1;33m"
#define LOGGER_FMT_RED       "\33[1;31m"
#define LOGGER_FMT_NONE      "\33[0m"

#define log(format, ...)                                                       \
  do {                                                                         \
    fprintf(stdout, LOGGER_FMT_BOLD format LOGGER_FMT_NONE, ## __VA_ARGS__);   \
  } while(0)

#define logln(format, ...)                                                     \
  do {                                                                         \
    fprintf(stdout,                                                            \
      LOGGER_FMT_BOLD format "\n"                                              \
      LOGGER_FMT_NONE,                                                         \
      ## __VA_ARGS__);                                                         \
  } while(0)

#define logc(...)                                                              \
  do {                                                                         \
    fprintf(stdout,                                                            \
      LOGGER_FMT_BOLD LOGGER_FMT_UNDERLINE "common"                            \
      LOGGER_FMT_NONE " %s:%s:%d\n", __FILE__, __func__, __LINE__);            \
    logln(__VA_ARGS__);                                                        \
  } while(0)

#define logi(...)                                                              \
  do {                                                                         \
    fprintf(stdout,                                                            \
      LOGGER_FMT_BOLD LOGGER_FMT_UNDERLINE LOGGER_FMT_GREEN "info"             \
      LOGGER_FMT_NONE " %s:%s:%d\n", __FILE__, __func__, __LINE__);            \
    logln(__VA_ARGS__);                                                        \
  } while(0)

#define logw(...)                                                              \
  do {                                                                         \
    fprintf(stdout,                                                            \
      LOGGER_FMT_BOLD LOGGER_FMT_UNDERLINE LOGGER_FMT_YELLOW "warning"         \
      LOGGER_FMT_NONE " %s:%s:%d\n", __FILE__, __func__, __LINE__);            \
    logln(__VA_ARGS__);                                                        \
  } while(0)

#define logd(...)                                                              \
  do {                                                                         \
    fprintf(stdout,                                                            \
      LOGGER_FMT_BOLD LOGGER_FMT_UNDERLINE LOGGER_FMT_GREEN "debug"            \
      LOGGER_FMT_NONE " %s:%s:%d\n", __FILE__, __func__, __LINE__);            \
    logln(__VA_ARGS__);                                                        \
  } while(0)

#define loge(...)                                                              \
  do {                                                                         \
    fprintf(stdout, LOGGER_FMT_BOLD LOGGER_FMT_UNDERLINE LOGGER_FMT_RED "error"\
      LOGGER_FMT_NONE " %s:%s:%d\n", __FILE__, __func__, __LINE__);            \
    logln(__VA_ARGS__);                                                        \
  } while(0)

#endif
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <assert.h>

#ifdef __DEBUG__

#define Assert(cond, ...)                                                      \
	do {                                                                         \
		if(!(cond)) {                                                              \
			fflush(stdout);                                                          \
			fprintf(stderr, "\33[1;31m");                                            \
			fprintf(stderr, __VA_ARGS__);                                            \
			fprintf(stderr, "\33[0m\n");                                             \
			assert(cond);                                                            \
		}                                                                          \
	} while(0)

#define panic(format, ...)                                                     \
	Assert(0, format, ## __VA_ARGS__)

#else

#define Assert(...)                                                            \
	do {} while(0)

#define panic(...)                                                             \
	do {} while(0)

#endif

#endif
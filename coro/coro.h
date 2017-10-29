#ifndef __CORO_H__
#define __CORO_H__

/**
 * coro_ex_t is a function to coroutine
 */
typedef void* (*coro_ex_t)(void*);

/**
 * coro will create a coroutine and make it runnable
 * @param ex   executable function
 * @param args arguments passed to ex
 */
void coro(coro_ex_t ex, void *args);

/**
 * yield will yield CPU to other coroutines
 */
void yield();

#endif
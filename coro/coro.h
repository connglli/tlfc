#ifndef __CORO_H__
#define __CORO_H__

/**
 * coro_ex_t is the function executed by coroutine
 */
typedef void* (*coro_ex_t)(void*);

/**
 * coro creates and runs a coroutine of ex, just like the key word `go` 
 * in Golang
 * @param ex   exeutable function
 * @param args args of ex
 */
void coro(coro_ex_t ex, void *args);

/**
 * yield yields CPU to other coroutines, just like the key word `yield` 
 * in ECMA2015+ or python.
 */
void yield();

#endif
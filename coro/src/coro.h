#ifndef __CORO_H__
#define __CORO_H__

/**
 * CORO_EX defines a coro_ex_t
 * @param  ex_name name of this coro_ex_t
 * @param  ex_args args of this coro_ex_t
 * @return         definition of this coro_ex_t
 */
#define CORO_EX(ex_name, ex_args) void* ex_name(void *ex_args)

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
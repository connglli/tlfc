#ifndef __GENT_H__
#define __GENT_H__

#include <stdlib.h>

// __GENT_CONTEXT_T is the internal context type of a generator
#define __GENT_CONTEXT_T         __gent_context_t
// __GENT_STATE_PLACEHOLDER is the placeholder for the internal state
#define __GENT_STATE_PLACEHOLDER __gent_state_placeholder
// __GENT_ARG_CONTEXT is the first arg of any GENT
#define __GENT_ARG_CONTEXT       __gent_arg_context

/**
 * gent_handler_t is the handler of a generator
 */
typedef void* gent_handler_t;

/**
 * GENT_HANDLER_INIT initializes a gent_handler_t
 * @param  hdlr hdlr to be initialized
 */
#define GENT_HANDLER_INIT(hdlr)                                                \
  do { *(hdlr) = NULL; } while(0)

/**
 * GENT declares an generator with return type return_t, generator name name,
 * and some args
 * @param  return_t return type of this generator
 * @param  name     name of this generator
 * @param  args     args of this generator
 */
#define GENT(return_t, name, args)                                             \
  return_t name(gent_handler_t *__GENT_ARG_CONTEXT, args)

/**
 * GENT_CONTEXT_BEGIN is the start notation of the context block
 */
#define GENT_CONTEXT_BEGIN                                                     \
  struct __GENT_CONTEXT_T {                                                    \
    int __GENT_STATE_PLACEHOLDER

/**
 * GENT_CONTEXT_END the end notation of the context block with ctx declared
 * @param  ctx context to be declared
 */
#define GENT_CONTEXT_END(ctx)                                                  \
  } *ctx = *(struct __GENT_CONTEXT_T**)__GENT_ARG_CONTEXT

/**
 * GENT_EX_BEGIN is the start notation of the ex block on context ctx
 * @param  ctx ctx to be declared
 */
#define GENT_EX_BEGIN(ctx)                                                     \
  if (NULL == (ctx)) {                                                         \
    (ctx) = *__GENT_ARG_CONTEXT = malloc(sizeof(struct __GENT_CONTEXT_T));     \
    (ctx)->__GENT_STATE_PLACEHOLDER = __LINE__;                                \
  }                                                                            \
  switch((ctx)->__GENT_STATE_PLACEHOLDER) { case __LINE__:

/**
 * GENT_YIELD does three things:
 *   1. saves the state to current line (for simplicity)
 *   2. returns v to caller
 *   3. sets the entry (the case statement) for subsequent entering
 * @param  v value to be returned
 */
#define GENT_YIELD(v)                                                          \
  do {                                                                         \
    (*(struct __GENT_CONTEXT_T**)__GENT_ARG_CONTEXT)->__GENT_STATE_PLACEHOLDER \
      = __LINE__;                                                              \
    return (v);                                                                \
    case __LINE__:;                                                            \
  } while(0)

/**
 * GENT_EX_END is the end notation of the ex block
 */
#define GENT_EX_END }

/**
 * GENT_RETURN terminates the generator with return value v
 *   1. you can call GENT_RETURN at any place in an ex block
 *   2. you must call GENT_RETURN whenever you terminates a generator, even
 *      though you are out of the ex block
 * @param  v 
 * @return   [description]
 */
#define GENT_RETURN(v)                                                         \
  do {                                                                         \
    free(*(struct __GENT_CONTEXT_T**)__GENT_ARG_CONTEXT);                      \
    *(struct __GENT_CONTEXT_T**)__GENT_ARG_CONTEXT = 0;                        \
    return (v);                                                                \
  } while(0)

#endif

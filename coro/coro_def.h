#ifndef __CORO_DEF_H__
#define __CORO_DEF_H__

#include "context.h"
#include "coro.h"
#include "list.h"
#include "pool.h"

/**
 * coro_t represents an executable coroutine
 */
typedef struct coro_t           coro_t;

/**
 * coro_handler_t represents handler to a coro_t
 */
typedef pool_fish_t             coro_handler_t;

/**
 * coro_scheduler_t is the scheduler of coroutines
 */
typedef struct coro_scheduler_t coro_scheduler_t;


/**
 * CORO_HANDLER_T_NULL is the zero of coro_handler_t
 */
#define CORO_HANDLER_T_NULL -1

/**
 * CORO_DEFAULT_STACK_SIZE is the stack size of every coroutine: 1MB
 */
#define CORO_DEFAULT_STACK_SIZE (1 << 20)

/**
 * CORO_DEFAULT_NR_CORO is the default number of coros: 16
 */
#define CORO_DEFAULT_NR_CORO    (1 << 4)

/**
 * CORO_STATE represents the state of a coro_t
 */
#define CORO_STATE_READY   0
#define CORO_STATE_ACTIVE  1
#define CORO_STATE_SUSPEND 2
#define CORO_STATE_ZOMBIE  3
#define CORO_STATE_DEAD    4


/**
 * struct coro_t represents an executable coroutine
 */
struct coro_t {
  char              *stack;     // stack
  int                stacksz;   // stack size
  ucontext_t         uctxt;     // context
  coro_handler_t     hdlr;      // pool handler
  int                id;        // unique id
  int                state;     // state
  coro_scheduler_t  *scheduler; // scheduler
  coro_ex_t          ex;        // exec function
  void              *args;      // args of exec function
};

/**
 * define a coro_handler_t list
 */
list_template(coro_t*, list_coro_t, list_coro_node_t);

/**
 * define a coro_t pool
 */
pool_template(coro_t, pool_coro_t);

/**
 * struct coro_scheduler_t is the scheduler of coroutines
 */
struct coro_scheduler_t {
  int          idgen;          // id generator
  ucontext_t   scheduler_ctxt; // scheduler context
  coro_t      *curr_coro;      // current coro
  list_coro_t  coro_queue;     // suspended coro
  pool_coro_t  coro_pool;      // a pool to fast get a coro
};

#endif
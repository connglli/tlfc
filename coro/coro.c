#include <stdlib.h>
#include <assert.h>
#include "coro_def.h"
#include "coro.h"

// declarations
// ====

extern coro_scheduler_t* coro_global_scheduler();
static inline void _coro_scheduler_executor(uint32_t ps_l32, uint32_t ps_h32);

// static functions
// ====

// _coro_to_coro_handler converts a pool_fish_t to coro_handler_t
#define _coro_to_coro_handler(fish)                                            \
  ( (coro_handler_t)(fish) )

// _coro_to_pool_fish converts a coro_handler_t to pool_fish_t
#define _coro_to_pool_fish(c_hdlr)                                             \
  ( (pool_fish_t)(c_hdlr) )

// _coro_scheduler_get_coro gets the coro of handler h
#define _coro_scheduler_get_coro(s, h)                                         \
  ( pool_getp(&(s)->coro_pool, _coro_to_pool_fish(h)) )

// _coro_switch_to_scheduler_from switch context to scheduler from coro c
#define _coro_switch_to_scheduler_from(s, c)                                   \
  do { swapcontext(&(c)->uctxt, &(s)->scheduler_ctxt); } while(0)

// _coro_switch_from_scheduler_to switch context to coro c from scheduler
#define _coro_switch_from_scheduler_to(s, c)                                   \
  do { swapcontext(&(s)->scheduler_ctxt, &(c)->uctxt); } while(0)

// _coro_scheduler_genid generates id
#define _coro_scheduler_genid(s)                                               \
  ( (s)->idgen ++ )

// _coro_init initializes a coro_t* c, with its corresponding scheduler, id,
// its handler, ex, args, stack, and stacksz
static inline
coro_t* _coro_init(coro_t* c, 
                   coro_scheduler_t* s, 
                   int id, coro_handler_t c_hdlr,
                   coro_ex_t ex, void *args, 
                   char *stack, int stacksz) {
  assert(NULL != c && 0 != s);
  assert(NULL != stack && 0 != stacksz);
  
  // id, hdlr, state
  c->id    = id;
  c->hdlr  = c_hdlr;
  c->state = CORO_STATE_READY;
    
  // exec
  c->ex   = ex;
  c->args = args;

  // stack
  c->stack   = stack;
  c->stacksz = stacksz;

  // context
  uintptr_t ps      = (uintptr_t)s;
  uint32_t  ps_l32  = (uint32_t)(ps);
  uint32_t  ps_h32  = (uint32_t)(ps >> 32);
  getcontext(&c->uctxt);
  // uc_link does not work
  // c->uctxt.uc_link          = &scheduler->scheduler_ctxt;
  c->uctxt.uc_stack.ss_sp   = c->stack;
  c->uctxt.uc_stack.ss_size = c->stacksz;
  makecontext(&c->uctxt, _coro_scheduler_executor, 2, ps_l32, ps_h32);

  return c;
}

// _coro_scheduler_pick_coro determines which coro will be executed next
static inline
coro_t* _coro_scheduler_pick_coro(coro_scheduler_t *s) {
  assert(NULL != s);
  if (list_size(&s->coro_queue) == 0) { return NULL; }

  // coro scheduling obeys FCFS
  coro_t *c = list_node_data(list_head(&s->coro_queue));
  assert(NULL != c);

  return c;
}

// _coro_scheduler_serve_coro makes c to be runned by s immediately
static inline
void _coro_scheduler_serve_coro(coro_scheduler_t *s, 
                                coro_t *c) {
  assert(NULL != s);
  // we only serve the fist by FCFS
  assert(_coro_scheduler_pick_coro(s) == c);

  // remove c from queue and change its state
  list_remove(&s->coro_queue, 0);
  c->state = CORO_STATE_ACTIVE;

  // run it
  s->curr_coro = c;
  _coro_switch_from_scheduler_to(s, c);
}

// _coro_scheduler_queue_coro queues c in s, with state
static inline
void _coro_scheduler_queue_coro(coro_scheduler_t *s, 
                                coro_t *c, int state) {
  assert(NULL != s && NULL != c);
  assert(CORO_STATE_READY == state || CORO_STATE_SUSPEND == state);

  // add c to queue and change its state 
  list_append(&s->coro_queue, c);
  c->state = state;
}

// _coro_scheduler_push_coro blews out a coro from pool for a new coro
static inline
coro_handler_t _coro_scheduler_push_coro(coro_scheduler_t *s) {
  assert(NULL != s);

  pool_fish_t fish;

  pool_fish(&s->coro_pool, &fish);
  if (!pool_is_fish_in_pool(&s->coro_pool, fish)) { 
    pool_expand_to_cap(&s->coro_pool, 2 * pool_capacity(&s->coro_pool));

    pool_fish(&s->coro_pool, &fish);
    assert(pool_is_fish_in_pool(&s->coro_pool, fish));
  }

  // revive it
  pool_qrevive(&s->coro_pool, fish);

  return _coro_to_coro_handler(fish);
}

// _coro_scheduler_pull_coro swallows a coro back to pool, and free its stack
static inline
void _coro_scheduler_pull_coro(coro_scheduler_t *s, coro_handler_t c_hdlr) {
  assert(NULL != s);
  assert(pool_is_fish_in_pool(&s->coro_pool, _coro_to_pool_fish(c_hdlr)));

  // free its stack
  coro_t *c = _coro_scheduler_get_coro(s, c_hdlr);
  free(c->stack);

  pool_kill(&s->coro_pool, _coro_to_pool_fish(c_hdlr));
}

// _coro_scheduler_coro is the real coro(), it wrapps ex, and queues it
static inline
void _coro_scheduler_coro(coro_scheduler_t *s, coro_ex_t ex, void *args) {
  coro_handler_t c_hdlr  = _coro_scheduler_push_coro(s);
  coro_t        *c       = _coro_scheduler_get_coro(s, c_hdlr);
  char          *stack   = c->stack;
  int            stacksz = c->stacksz;

  if (NULL == c->stack) {
    stack = (char*)malloc(CORO_DEFAULT_STACK_SIZE);
    if (NULL == stack) { assert(0); }
    stacksz = CORO_DEFAULT_STACK_SIZE;
  }

  _coro_init(c,
    s, 
    _coro_scheduler_genid(s), c_hdlr,
    ex, args, 
    stack, stacksz);

  _coro_scheduler_queue_coro(s, c, CORO_STATE_READY);
}

// _coro_scheduler_yield is the real yield(), it switches context to scheduler
static inline
void _coro_scheduler_yield(coro_scheduler_t *s) {
  coro_t *yielded_coro = s->curr_coro;

  s->curr_coro = NULL;
  _coro_scheduler_queue_coro(s, yielded_coro, CORO_STATE_SUSPEND);

  _coro_switch_to_scheduler_from(s, yielded_coro);
}

// _coro_scheduler_executor is the wrapper of ex, it manages the lifespan of ex
static inline
void _coro_scheduler_executor(uint32_t ps_l32, uint32_t ps_h32) {
  uintptr_t ps = ps_l32 | (uintptr_t)ps_h32 << 32;
  coro_scheduler_t *s = (coro_scheduler_t*)ps;

  coro_t *curr_coro = s->curr_coro;
  assert(curr_coro != NULL);

  coro_ex_t ex = curr_coro->ex;
  void *args = curr_coro->args;

  // execute it
  ex(args);

  curr_coro->state = CORO_STATE_ZOMBIE;
  s->curr_coro = NULL;

  curr_coro->state = CORO_STATE_DEAD;
  _coro_scheduler_pull_coro(s, curr_coro->hdlr);

  // !!! IMPORTRANT 
  // uc_link does not work for the implementation of libtask/context,
  // so we need to switch context manually
  _coro_switch_to_scheduler_from(s, curr_coro);
}

// coro implementation
// ====

/**
 * coro_scheduler_init initializes a coro_scheduler_t
 * @param s ptr to a coro_scheduler_t
 */
void coro_scheduler_init(coro_scheduler_t *s) {
  assert(NULL != s);
  s->idgen     = 0;
  s->curr_coro = NULL;
  list_init(&s->coro_queue);
  pool_init(&s->coro_pool, CORO_DEFAULT_NR_CORO);
}

/**
 * coro_scheduler_run runs a coro_scheduler_t
 * @param s ptr to a coro_scheduler_t
 */
void coro_scheduler_run(coro_scheduler_t *s) {
  assert(NULL != s);

  do {
    // every schedule must be scheduled by scheduler
    assert(NULL == s->curr_coro);

    coro_t *c = _coro_scheduler_pick_coro(s);

    // no coro is in the queue, exit the program
    if (NULL == c) { break; }

    switch (c->state) {
      case CORO_STATE_READY:
      case CORO_STATE_SUSPEND:
        _coro_scheduler_serve_coro(s, c);
        break;

      case CORO_STATE_ACTIVE: 
      case CORO_STATE_ZOMBIE: 
      case CORO_STATE_DEAD:
        /* cannot be these */
        assert(0);
        break;

      default:
        /* never reach here */
        assert(0);
    }
  } while(1);
}

/**
 * coro_scheduler_deinit destroys a coro_scheduler_t
 * @param s ptr to a coro_scheduler_t
 */
void coro_scheduler_deinit(coro_scheduler_t *s) {
  if (NULL == s) { return; }
  list_deinit(&s->coro_queue);
  pool_deinit(&s->coro_pool);
  s->curr_coro = NULL;
  s->idgen     = 0;
}

/**
 * coro creates and runs a coroutine of ex, just like the key word `go` 
 * in Golang
 * @param ex   exeutable function
 * @param args args of ex
 */
void coro(coro_ex_t ex, void *args) {
  _coro_scheduler_coro(coro_global_scheduler(), ex, args);
}

/**
 * yield yields CPU to other coroutines, just like the key word `yield` 
 * in ECMA2015+ or python.
 */
void yield() {
  _coro_scheduler_yield(coro_global_scheduler());
}
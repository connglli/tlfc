#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include "def.h"
#include "coro.h"
#include "mailbox.h"

// declarations
// ====

extern coro_scheduler_t* coro_global_scheduler();
static void _coro_scheduler_executor(uint32_t ps_l32, uint32_t ps_h32);

// static functions
// ====

// _coro_to_coro_handler converts a pool_fish_t to coro_handler_t
#define _coro_to_coro_handler(fish)                                            \
  ( (coro_handler_t)(fish) )

// _coro_to_pool_fish converts a coro_handler_t to pool_fish_t
#define _coro_to_pool_fish(c_hdlr)                                             \
  ( (pool_fish_t)(c_hdlr) )

// _coro_scheduler_find_coro_by_hdlr gets the coro of handler h
#define _coro_scheduler_find_coro_by_hdlr(s, h)                                \
  ( pool_getp(&(s)->coro_pool, _coro_to_pool_fish(h)) )

// _coro_switch_to_scheduler_from switch context to scheduler from coro c
#define _coro_switch_to_scheduler_from(s, c)                                   \
  do { swapcontext(&(c)->uctxt, &(s)->scheduler_ctxt); } while(0)

// _coro_switch_from_scheduler_to switch context to coro c from scheduler
#define _coro_switch_from_scheduler_to(s, c)                                   \
  do { swapcontext(&(s)->scheduler_ctxt, &(c)->uctxt); } while(0)

// _coro_scheduler_has_delayed_coros determine whether has delayed coros
#define _coro_scheduler_has_delayed_coros(s)                                   \
  ( list_size(&(s)->delayed_coros) != 0 )

// _coro_scheduler_genid generates id
#define _coro_scheduler_genid(s)                                               \
  ( (s)->idgen ++ )

// _coro_init initializes a coro_t* c, with its corresponding scheduler, id,
// its handler, ex, args, stack, and stacksz
static
coro_t* _coro_init(coro_t *c,
                   coro_scheduler_t *s,
                   coro_id_t id, coro_handler_t c_hdlr,
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

  // mailbox
  _coro_mailbox_init(&c->mailbox);

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

// _coro_deinit deinits a coro_t
static
void _coro_deinit(coro_t *c) {
  free(c->stack); c->stack = NULL;
  _coro_mailbox_deinit(&c->mailbox);
}

// _coro_scheduler_find_coro_by_id find the coro with id cid
static
coro_t* _coro_scheduler_find_coro_by_id(coro_scheduler_t *s, coro_id_t id) {
  assert(NULL != s);

  coro_t           *c = NULL;
  list_coro_node_t *n = NULL;
  list_foreach_n(&s->suspended_coros, n) {
    if (list_node_data(n)->id == id) { c = list_node_data(n); return c; }
  }

  list_coro_delayed_node_t *nn = NULL;
  list_foreach_n(&s->delayed_coros, nn) {
    if (list_node_data(nn).c->id == id) { c = list_node_data(nn).c; return c; }
  }

  return c;
}

// _coro_scheduler_serve_coro makes c to be runned by s immediately
static
void _coro_scheduler_serve_coro(coro_scheduler_t *s, 
                                coro_t *c) {
  assert(NULL != s);

  // change its state
  c->state = CORO_STATE_ACTIVE;

  // run it
  s->curr_coro = c;
  _coro_switch_from_scheduler_to(s, c);
}

// _coro_scheduler_queue_coro queues c in s, with state
static
void _coro_scheduler_queue_coro(coro_scheduler_t *s, 
                                coro_t *c, int state) {
  assert(NULL != s && NULL != c);
  assert(CORO_STATE_READY == state || CORO_STATE_SUSPENDED == state);

  // add c to queue and change its state 
  list_append(&s->suspended_coros, c);
  c->state = state;
}

// _coro_scheduler_suspend_coro suspended c in s, with state CORO_STATE_SUSPENDED
static inline
void _coro_scheduler_suspend_coro(coro_scheduler_t *s,  coro_t *c) {
  _coro_scheduler_queue_coro(s, c, CORO_STATE_SUSPENDED);
}

// _coro_scheduler_prepare_coro prepares c in s, with state CORO_STATE_READY
static inline
void _coro_scheduler_prepare_coro(coro_scheduler_t *s,  coro_t *c) {
  _coro_scheduler_queue_coro(s, c, CORO_STATE_READY);
}

// _coro_scheduler_squeeze_coro blews out a coro from pool for a new coro
static
coro_handler_t _coro_scheduler_squeeze_coro(coro_scheduler_t *s) {
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

// _coro_scheduler_absorb_coro swallows a coro back to pool, and free its stack
static
void _coro_scheduler_absorb_coro(coro_scheduler_t *s, coro_handler_t c_hdlr) {
  assert(NULL != s);
  assert(pool_is_fish_in_pool(&s->coro_pool, _coro_to_pool_fish(c_hdlr)));

  // deinit c
  coro_t *c = _coro_scheduler_find_coro_by_hdlr(s, c_hdlr);
  _coro_deinit(c);

  pool_kill(&s->coro_pool, _coro_to_pool_fish(c_hdlr));
}

// _coro_scheduler_coro is the real coro(), it wrapps ex, and queues it
static
coro_id_t _coro_scheduler_coro(coro_scheduler_t *s, coro_ex_t ex, void *args) {
  coro_handler_t c_hdlr  = _coro_scheduler_squeeze_coro(s);
  coro_t        *c       = _coro_scheduler_find_coro_by_hdlr(s, c_hdlr);
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

  _coro_scheduler_prepare_coro(s, c);

  return c->id;
}

// _coro_scheduler_yield is the real yield(), it switches context to scheduler
static
void _coro_scheduler_yield(coro_scheduler_t *s) {
  coro_t *yielded_coro = s->curr_coro;

  s->curr_coro = NULL;
  _coro_scheduler_suspend_coro(s, yielded_coro);

  _coro_switch_to_scheduler_from(s, yielded_coro);
}

// _coro_scheduler_delay is the real delay(), it delays current coro and then
// switches context to scheduler
static
void _coro_scheduler_delay(coro_scheduler_t *s, long long time) {
  coro_t *delayed_coro = s->curr_coro;

  s->curr_coro = NULL;

  // construct a delayed coro
  coro_delayed_t d = { delayed_coro, 0, 0 };

  // get current time
  struct timeval tv;
  int ret = gettimeofday(&tv, NULL);
  if (ret != 0) { assert(0); }

  d.begin_at = tv.tv_sec * 1000;
  // negative means freeze it
  d.end_at = time < 0 ? -1 : (d.begin_at + time);

  // add c to delayed coros and change its state
  list_append(&s->delayed_coros, d);
  delayed_coro->state = CORO_STATE_DELAYED;

  _coro_switch_to_scheduler_from(s, delayed_coro);
}

// _coro_scheduler_undelay will make coroutine referenced by cid suspended
static
void _coro_scheduler_undelay(coro_scheduler_t *s, coro_id_t cid) {
  coro_t *c   = NULL;
  int     idx = 0;

  list_coro_delayed_node_t *n;
  list_foreach_n(&s->delayed_coros, n) {
    // find it, unfreeze it, and suspend it
    if (cid == list_node_data(n).c->id) {
      c = list_node_data(n).c;
      list_remove(&s->delayed_coros, idx);
      // undelayed ones have greater priority to other suspended ones
      c->state = CORO_STATE_SUSPENDED;
      list_prepend(&s->suspended_coros, c);
      break;
    }
    idx ++;
  }
}

// _coro_scheduler_freeze will make current coroutine delays forever
static inline
void _coro_scheduler_freeze(coro_scheduler_t *s) {
  _coro_scheduler_delay(s, -1);
}

// _coro_scheduler_unfreeze will make coroutine referenced by cid be unfrozen
static inline
void _coro_scheduler_unfreeze(coro_scheduler_t *s, coro_id_t cid) {
  _coro_scheduler_undelay(s, cid);
}

// _coro_scheduler_next determines which coro will be executed next
static
coro_t* _coro_scheduler_next(coro_scheduler_t *s) {
  assert(NULL != s);

  coro_t *c          = NULL;
  int     nr_delayed = list_size(&s->delayed_coros);

  // 1. we make a priority to delayed coroutines, find one which 
  //    firstly completes delayed task
  if (nr_delayed > 0) {
    // get current time
    struct timeval tv;
    int ret = gettimeofday(&tv, 0);
    if (ret != 0) { assert(0); } // error
    
    long long  exceeded_tm     = 0;
    long long  min_exceeded_tm = (((unsigned long long)-1) >> 1);
    long long  now             = tv.tv_sec * 1000;

    // we allocate nr_delay memories
    coro_id_t *exceeded_coro_ids  = (int*)calloc(nr_delayed, sizeof(coro_id_t));
    int  nr_exceeded              = 0;
    if (NULL == exceeded_coro_ids) { assert(0); }

    // 1.1 traverse, find and save exceeded ones and c
    list_coro_delayed_node_t *n;
    list_foreach_n(&s->delayed_coros, n) {
      // those which freeze
      if (list_node_data(n).end_at < 0) { continue; }

      // those which delay
      exceeded_tm = now - list_node_data(n).end_at;
      // delayed done
      if (exceeded_tm >= 0) {
        exceeded_coro_ids[nr_exceeded ++] = list_node_data(n).c->id;
        if (min_exceeded_tm > exceeded_tm) {
          min_exceeded_tm = exceeded_tm;
          c = list_node_data(n).c;
        }
      }
    }

    if (nr_exceeded > 0) {
      assert(NULL != c);

      // first undelay !c
      for (int i = 0; i < nr_exceeded; i ++) {
        if (exceeded_coro_ids[i] == c->id) { continue; }
        _coro_scheduler_undelay(s, exceeded_coro_ids[i]);
      }

      // then undelay c, and c will be the first suspended one
      _coro_scheduler_undelay(s, c->id);    
    } else {
      assert(NULL == c);
    }

    free(exceeded_coro_ids);
  }

  // 2. no delayed coroutines is ready, we pick a suspended one according to FCFS
  //    if we find one in 1(above), it will be the 1st one of suspended coros
  if (0 == list_size(&s->suspended_coros)) { return NULL; }

  // remove c from suspended coros
  c = list_node_data(list_head(&s->suspended_coros));
  assert(NULL != c);
  list_remove(&s->suspended_coros, 0);

  return c;
}

// _coro_scheduler_executor is the wrapper of ex, it manages the lifespan of ex
static
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
  _coro_scheduler_absorb_coro(s, curr_coro->hdlr);

  // !!! IMPORTRANT 
  // uc_link does not work for the implementation of libtask/context,
  // so we need to switch context manually
  _coro_switch_to_scheduler_from(s, curr_coro);
}

// coro implementation
// ====

/*----------------------------------------------------------------------------
                                    scheduler
  ----------------------------------------------------------------------------*/

/**
 * coro_scheduler_init initializes a coro_scheduler_t
 * @param s ptr to a coro_scheduler_t
 */
void coro_scheduler_init(coro_scheduler_t *s) {
  assert(NULL != s);
  s->idgen     = 0;
  s->curr_coro = NULL;
  list_init(&s->suspended_coros);
  list_init(&s->delayed_coros);
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

    coro_t *c = _coro_scheduler_next(s);

    // no coro is suspended, but there are delayed
    if (NULL == c && _coro_scheduler_has_delayed_coros(s)) { continue; }
    // no more coros, exit the program
    else if (NULL == c) { break; }

    switch (c->state) {
      case CORO_STATE_READY:
      case CORO_STATE_SUSPENDED:
        _coro_scheduler_serve_coro(s, c);
        break;

      case CORO_STATE_ACTIVE:
      case CORO_STATE_DELAYED:
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
  list_deinit(&s->suspended_coros);
  list_deinit(&s->delayed_coros);
  pool_deinit(&s->coro_pool);
  s->curr_coro = NULL;
  s->idgen     = 0;
}

/*----------------------------------------------------------------------------
                                    coroutine
  ----------------------------------------------------------------------------*/

/**
 * coro creates and queues a coroutine of ex, does not run immediately
 * @param ex   exeutable function
 * @param args args of ex
 * @return     id of this coroutine
 */
coro_id_t coro(coro_ex_t ex, void *args) {
  return _coro_scheduler_coro(coro_global_scheduler(), ex, args);
}

/**
 * yield yields CPU to other coroutines, just like the key word `yield` 
 * in ECMA2015+ or python.
 */
void yield() {
  _coro_scheduler_yield(coro_global_scheduler());
}

/**
 * delay makes coroutine delay executing for at least time ms
 * @param time sleeping time(millisecond)
 */
void delay(long long time) {
  if (0 >= time) { yield(); }
  else { _coro_scheduler_delay(coro_global_scheduler(), time); }
}

/**
 * self returns id of this coroutine
 * @return id of this coroutine
 */
coro_id_t self() {
  return coro_global_scheduler()->curr_coro->id;
}

/**
 * freeze will make current coroutine delays forever
 */
void freeze() {
  _coro_scheduler_freeze(coro_global_scheduler());
}

/**
 * unfreeze will make coroutine referenced by cid be unfrozen
 */
void unfreeze(coro_id_t cid) {
  _coro_scheduler_unfreeze(coro_global_scheduler(), cid);
}

/*----------------------------------------------------------------------------
                                    messaging
  ----------------------------------------------------------------------------*/

/**
 * send sends a message to cid
 * @param  cid  id of the receiver
 * @param  type message type
 * @param  data message data
 * @return      0 when succeeded else -1
 */
int send(coro_id_t cid, int type, void *data) {
  coro_scheduler_t *s = coro_global_scheduler();
  coro_t           *c = _coro_scheduler_find_coro_by_id(s, cid);

  if (NULL == c) { return -1; }

  coro_mail_t m = { self(), cid, type, data };

  // all coroutine runs in one thread, so we do not need any lock,
  // or, we have to add a lock member to struct mailbox.
  _coro_mailbox_put_mail(&c->mailbox, m);

  return 0;
}

/**
 * receive receives a message from other coroutine: 
 *  if there is a message, then return immediately;
 *  else if timeout > 0, then block until there is a message within timeout;
 *  else if timeout = 0, then return immediately;
 *  else block until there is a message
 * @param  timeout blocked time(millisecond)
 * @return         message received(or the message type will be a negative number)
 */
coro_msg_t receive(long long timeout) {
  coro_scheduler_t *s         = coro_global_scheduler();
  coro_t           *curr_coro = s->curr_coro;
  coro_mailbox_t   *mb        = &curr_coro->mailbox;
  coro_msg_t        m         = { 0, 0, -1, NULL };

  // there is a message, return immediately
  if (!_coro_mailbox_is_empty(mb)) { 
    m = _coro_mailbox_get_mail(mb);
    return m;
  }

  // nonblock
  if (0 == timeout) { return m; }

  // block until there is a message
  if (timeout < 0) {
    while(_coro_mailbox_is_empty(mb)) {
      yield();
    }

    m = _coro_mailbox_get_mail(mb);

    return m;
  }

  // block until time out or a message is available
  struct timeval  tv;

  int ret = gettimeofday(&tv, NULL);
  if (ret != 0) { return m; } // error

  long long t_from      = tv.tv_sec * 1000;
  long long t_duration  = 0;

  do {
    yield();
    
    if (!_coro_mailbox_is_empty(mb)) { 
      m = _coro_mailbox_get_mail(mb);
      return m;
    }

    ret = gettimeofday(&tv, NULL);
    if (ret != 0) { return m; } // error

    t_duration = tv.tv_sec * 1000 - t_from;
  } while(t_duration < timeout);
  
  return m;
}
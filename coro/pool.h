#ifndef __POOL_H__
#define __POOL_H__

#include <stdlib.h>
#include <assert.h>

#define __cat2(a, b) a##b

/**
 * pool fish state can only changed by:
 * 
 *                         ->: pool_fish() 
 *               +-> DEAD <---------------> ZOMBIE
 *               |         <-: pool_kill()    ^ 
 *               |                            | ^ pool_zombiate()
 *   pool_kill() o                            o 
 *                \                          /  v pool_[q]revive()
 *                 `----<---- ALIVE <-------’
 */
#define POOL_FISH_STATE_ALIVE  1
#define POOL_FISH_STATE_ZOMBIE 2
#define POOL_FISH_STATE_DEAD   3

/**
 * pool_fish_t is the handler of a fish in pool
 */
typedef int pool_fish_t;

/**
 * pool_template declares a pool_t
 * @param  T           type to be declared
 * @param  pool_t      type name alias of pool
 * @return             a wrapped struct
 */
#define pool_template(T, pool_t)                                               \
  typedef struct __cat2(pool_fish, pool_t) __cat2(pool_fish, pool_t);          \
  typedef struct pool_t                    pool_t;                             \
  /* pool fish type */                                                         \
  struct __cat2(pool_fish, pool_t) {                                           \
    int state;                                                                 \
    int prev, next;                                                            \
    T   data;                                                                  \
  };                                                                           \
  /* pool type, we name it pool_t in the following */                          \
  struct pool_t {                                                              \
    __cat2(pool_fish, pool_t) *fishes;                                         \
    int                        alive_head;                                     \
    int                        alive_tail;                                     \
    int                        dead_head;                                      \
    int                        alive_size;                                     \
    int                        dead_size;                                      \
    int                        capacity;                                       \
  } ;

/**
 * pool_init initializes a pool_t, if failed the program will crash
 * @param  p   ptr to a pool_t
 * @param  cap capacity
 */
#define pool_init(p, cap)                                                      \
  do {                                                                         \
    if (cap > 0) {                                                             \
      (p)->fishes = (void*)malloc((cap) * sizeof(*(p)->fishes));               \
      if ((p)->fishes == NULL) { assert(0); }                                  \
      memset((p)->fishes, 0, (cap) * sizeof(*(p)->fishes));                    \
      for (int i = 0; i < (cap); i ++) {                                       \
        (p)->fishes[i].state = POOL_FISH_STATE_DEAD;                           \
        (p)->fishes[i].prev  = i - 1;                                          \
        (p)->fishes[i].next  = i + 1;                                          \
      }                                                                        \
      (p)->fishes[0].prev         = -1;                                        \
      (p)->fishes[(cap) - 1].next = -1;                                        \
      (p)->capacity               = (cap);                                     \
      (p)->alive_head             = -1;                                        \
      (p)->alive_tail             = -1;                                        \
      (p)->alive_size             = 0;                                         \
      (p)->dead_head              = 0;                                         \
      (p)->dead_size              = (cap);                                     \
    } else {                                                                   \
      (p)->fishes     = NULL;                                                  \
      (p)->capacity   = 0;                                                     \
      (p)->alive_head = -1;                                                    \
      (p)->alive_tail = -1;                                                    \
      (p)->alive_size = 0;                                                     \
      (p)->dead_head  = -1;                                                    \
      (p)->dead_size  = 0;                                                     \
    }                                                                          \
  } while(0)

/**
 * pool_dead_size gets the still usable size a pool_t
 * @param  p ptr to a pool_t
 * @return   the still usable size of p
 */
#define pool_dead_size(p)                                                      \
  ( (p)->dead_size )

/**
 * pool_capacity gets capacity of a pool_t
 * @param  p ptr to a pool_t
 * @return   capacity of p
 */
#define pool_capacity(p)                                                       \
  ( (p)->capacity )

/**
 * pool_is_fish_alive checks if a pool_fish_t is alive
 * @param  p    ptr to a pool_t
 * @param  fish pool_fish_t to be checked
 * @return      true if alive else false
 */
#define pool_is_fish_alive(p, fish)                                            \
  ( pool_is_fish_in_pool((p), (fish))                                          \
    ? ((p)->fishes[(fish)].state == POOL_FISH_STATE_ALIVE ? 1 : 0)             \
    : (0) )

/**
 * pool_is_fish_in_pool checks if a pool_fish_t is valid
 * @param  p    ptr to a pool_t
 * @param  fish pool_fish_t to be checked
 * @return      true if valid else false
 */
#define pool_is_fish_in_pool(p, fish)                                          \
  ( 0 <= (fish) && (fish) < (p)->capacity )

/**
 * pool_safe_fish returns if a pool_fish_t is safe, or crash the program
 * @param  p    ptr to a pool_t
 * @param  fish pool_fish_t to be checked
 * @return      fish if safe
 */
#define pool_safe_fish(p, fish)                                                \
  ( pool_is_fish_in_pool((p), (fish)) ? (fish) : (assert(0), 0) )

/**
 * pool_fish_state returns state of a pool_fish_t
 * @param  p    ptr to a pool_t
 * @param  fish pool_fish_t to be checked state
 * @return      state of this pool_fish_t
 */
#define pool_fish_state(p, fish)                                               \
  ( (p)->fishes[pool_safe_fish((p), (fish))].state )

/**
 * pool_get gets data via a alive pool_fish_t, or crash the program
 * @param  p    ptr to a pool_t
 * @param  fish pool_fish_t to be queried
 * @return      data of fish
 */
#define pool_get(p, fish)                                                      \
  ( pool_safe_fish((p), (fish)) == (fish)                                      \
    ? ((p)->fishes[(fish)].state == POOL_FISH_STATE_ALIVE                      \
       ? (p)->fishes[(fish)].data                                              \
       : (assert(0), (0)))                                                     \
    : (0) )

/**
 * pool_getp gets ptr to data via a alive pool_fish_t, or crash the program
 * @param  p    ptr to a pool_t
 * @param  fish pool_fish_t to be queried
 * @return      ptr to data of fish
 */
#define pool_getp(p, fish)                                                      \
  ( pool_safe_fish((p), (fish)) == (fish)                                      \
    ? ((p)->fishes[(fish)].state == POOL_FISH_STATE_ALIVE                      \
       ? &((p)->fishes[(fish)].data)                                           \
       : (assert(0), (NULL)))                                                     \
    : (NULL) )

/**
 * pool_set sets data via a alive pool_fish_t, or crash the program
 * @param  p    ptr to a pool_t
 * @param  fish pool_fish_t to be queried
 * @param  x    value to be set
 */
#define pool_set(p, fish, x)                                                   \
  do {                                                                         \
    if (pool_safe_fish((p), (fish)) == (fish)) {                               \
      if ((p)->fishes[(fish)].state == POOL_FISH_STATE_ALIVE) {                \
        (p)->fishes[(fish)].data = (x);                                        \
      } else {                                                                 \
        assert(0);                                                             \
      }                                                                        \
    }                                                                          \
  } while(0)

/**
 * pool_fish fishes for an available pool_fish_t, makes it a ZOMBIE
 * @param  p ptr to a pool_t
 * @param  f place to push new fish, of type pool_fish_t*
 * @return   positive pool_fish_t if available else negative
 */
#define pool_fish(p, f)                                                        \
  do {                                                                         \
    if((p)->dead_head == -1 ) { *(f) = -1; break; }                            \
    *(f) = (p)->dead_head;                                                     \
    (p)->dead_head = (p)->fishes[*(f)].next;                                   \
    (p)->fishes[*(f)].state = POOL_FISH_STATE_ZOMBIE;                          \
    (p)->fishes[*(f)].next = -1;                                               \
    (p)->fishes[*(f)].prev = -1;                                               \
    (p)->dead_size --;                                                         \
  } while(0)

/**
 * pool_revive revives a pool_fist_t using x, makes it from ZOMBIE to ALIVE
 * @param  p    ptr to a pool_t
 * @param  fish a pool_fish_t to be revived
 * @param  x    revive fish using x
 * @return      fish if succeeded else -1
 */
#define pool_revive(p, fish, x)                                                \
  ( pool_is_fish_in_pool((p), (fish)) &&                                       \
      (p)->fishes[(fish)].state == POOL_FISH_STATE_ZOMBIE                      \
    ? ((p)->fishes[(fish)].state = POOL_FISH_STATE_ALIVE,                      \
       (p)->fishes[(fish)].data = (x),                                         \
       (p)->fishes[(fish)].prev = (p)->alive_tail,                             \
       (p)->fishes[(fish)].next = -1,                                          \
       ((p)->alive_tail == -1                                                  \
        ? ((p)->alive_head = (fish))                                           \
        : ((p)->fishes[(p)->alive_tail].next = (fish))),                       \
       (p)->alive_tail = (fish),                                               \
       (p)->alive_size ++,                                                     \
       (fish))                                                                 \
    : (-1) )

/**
 * pool_qrevive quickly revives a pool_fist_t, makes it from ZOMBIE to ALIVE
 * @param  p    ptr to a pool_t
 * @param  fish a pool_fish_t to be revived
 * @return      fish if succeeded else -1
 */
#define pool_qrevive(p, fish)                                                  \
  ( pool_is_fish_in_pool((p), (fish)) &&                                       \
      (p)->fishes[(fish)].state == POOL_FISH_STATE_ZOMBIE                      \
    ? ((p)->fishes[(fish)].state = POOL_FISH_STATE_ALIVE,                      \
       (p)->fishes[(fish)].prev = (p)->alive_tail,                             \
       (p)->fishes[(fish)].next = -1,                                          \
       ((p)->alive_tail == -1                                                  \
        ? ((p)->alive_head = (fish))                                           \
        : ((p)->fishes[(p)->alive_tail].next = (fish))),                       \
       (p)->alive_tail = (fish),                                               \
       (p)->alive_size ++,                                                     \
       (fish))                                                                 \
    : (-1) )

/**
 * pool_zombiate makes a pool_fish_t from ALIVE to ZOMBIE
 * NEVER USE IT IN YOUR OWN CODES, OR UNDEFINED BEHAVIOR WILL BE GENERATED
 * @param  p    ptr to a pool_t
 * @param  fish a pool_fish_t to be zombiated
 */
#define pool_zombiate(p, fish)                                                 \
  do {                                                                         \
    if (!pool_is_fish_in_pool((p), (fish))) { break; }                         \
    if ((p)->fishes[(fish)].state != POOL_FISH_STATE_ALIVE) { break; }         \
    int prev = (p)->fishes[(fish)].prev;                                       \
    int next = (p)->fishes[(fish)].next;                                       \
    (p)->fishes[(fish)].state = POOL_FISH_STATE_ZOMBIE;                        \
    (p)->fishes[(fish)].next = -1;                                             \
    (p)->fishes[(fish)].prev = -1;                                             \
    if (prev == -1) { (p)->alive_head = next; }                                \
    else { (p)->fishes[prev].next = next; }                                    \
    if (next == -1) { (p)->alive_tail = prev; }                                \
    else { (p)->fishes[next].prev = prev; }                                    \
    (p)->alive_size --;                                                        \
 } while(0)

/**
 * pool_kill kills a pool_fish_t, makes it DEAD
 * @param  p    ptr to a pool_t
 * @param  fish a pool_fish_t to be killed
 */
#define pool_kill(p, fish)                                                     \
  do {                                                                         \
    if (!pool_is_fish_in_pool((p), (fish))) { break; }                         \
    if ((p)->fishes[(fish)].state == POOL_FISH_STATE_DEAD) { break; }          \
    if ((p)->fishes[(fish)].state == POOL_FISH_STATE_ALIVE) {                  \
      pool_zombiate((p), (fish));                                              \
    }                                                                          \
    (p)->fishes[(fish)].state = POOL_FISH_STATE_DEAD;                          \
    (p)->fishes[(fish)].next = (p)->dead_head;                                 \
    (p)->fishes[(fish)].prev = -1;                                             \
    if ((p)->dead_head != -1) { (p)->fishes[(p)->dead_head].prev = (fish); }   \
    (p)->dead_head = (fish);                                                   \
    (p)->dead_size ++;                                                         \
  } while(0)

/**
 * pool_expand expands the pool capacity to cap,
 * if the capacity is already greater or equal to cap, 
 *   then nothing happened,
 * else
 *   expands it to cap, if expand failed, the program will be crashed
 * @param  p   ptr to a pool_t
 * @param  cap cap to be expanded to
 */
#define pool_expand_to_cap(p, cap)                                             \
  do {                                                                         \
    if ((p)->capacity >= (cap)) { break; }                                     \
    (p)->fishes = realloc((p)->fishes, (cap) * sizeof(*(p)->fishes));          \
    if ((p)->fishes == NULL) { assert(0); }                                    \
    for (int i = (p)->capacity; i < (cap); i ++) {                             \
      (p)->fishes[i].state = POOL_FISH_STATE_DEAD;                             \
      (p)->fishes[i].prev  = i - 1;                                            \
      (p)->fishes[i].next  = i + 1;                                            \
    }                                                                          \
    (p)->fishes[(p)->capacity].prev = -1;                                      \
    (p)->fishes[(cap) - 1].next     = (p)->dead_head;                          \
    if ((p)->dead_head != -1) {                                                \
      (p)->fishes[(p)->dead_head].prev = (cap) - 1;                            \
    }                                                                          \
    (p)->dead_head = (p)->capacity;                                            \
    (p)->dead_size += (cap) - (p)->capacity;                                   \
    (p)->capacity = (cap);                                                     \
  } while(0)

/**
 * pool_deinit destroys a pool_t
 * @param  p ptr to a pool_t
 */
#define pool_deinit(p)                                                         \
  do { free((p)->fishes); pool_init((p), 0); } while(0)

#endif
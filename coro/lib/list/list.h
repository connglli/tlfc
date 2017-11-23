#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * list_template declares a list_t and a list_node_t
 * @param  T           type to be declared
 * @param  list_t      type name alias of list
 * @param  list_node_t type name alias of list node
 * @return             a wrapped struct
 */
#define list_template(T, list_t, list_node_t)                                  \
  typedef struct list_node_t list_node_t;                                      \
  typedef struct list_t      list_t;                                           \
  /* list type, we name it list_t in the folowing */                           \
  struct list_node_t {                                                         \
    list_node_t *prev, *next;                                                  \
    T            data;                                                         \
  };                                                                           \
  /* list node type, we name it list_t in the folowing */                      \
  struct list_t {                                                              \
    list_node_t *head;                                                         \
    list_node_t *tail;                                                         \
    int          size;                                                         \
  };

/**
 * list_struct declares an list_t
 * @param  NT type to be declared as node type
 * @return    a wrapped struct
 */
#define list_struct(NT)                                                        \
  struct { NT *head, *tail; int size; }

/**
 * list_unstruct unstructs the list directly to its inner data
 * @param  l ptr to a list_t
 * @return   inner datas
 */
#define list_unstruct(l)                                                       \
  ( (char**)&(l)->head),                                                       \
    ((char**)&(l)->tail),                                                      \
    (&(l)->size),                                                              \
    (sizeof(*(l)->head) )

/**
 * make_list makes an list for any list_t
 * @return an list_t
 */
#define make_list()                                                            \
  { NULL, NULL, 0 }

/**
 * list_node_init initializes a node
 * @param  n ptr to a list_node_t
 * @param  x value to be added
 */
#define list_node_init(n, x)                                                   \
  do { (n)->prev = (n)->next = n; (n)->data = (x); } while(0)

/**
 * list_init initializes a list
 * @param  l ptr to a list_t
 */
#define list_init(l)                                                           \
  do { memset((l), 0, sizeof(*(l))); } while(0)

/**
 * list_node_alloc allocates memories for a node of type NT
 * @param  NT node type(not pointer)
 * @return    ptr to a node of type NT
 */
#define list_node_alloc(NT)                                                    \
  ( (NT*)malloc(sizeof(NT)) )

/**
 * list_node_make allocates and then initializes a node of type NT
 * @param  NT node type(not pointer)
 * @param  x  value to be added
 * @return    ptr to a node of type NT
 */
#define list_node_make(NT, x)                                                  \
  ( (NT*)_list_node_make(sizeof(NT), &(x), sizeof(x)) )

/**
 * _list_node_unsafe_prevp gets ptr to prev(in type void**) from n(mostly void*)
 * @param  n ptr to a list_node_t
 * @return   ptr to prev of n, in type void**
 */
#define _list_node_unsafe_prevp(n)                                             \
  ( (void**)(n) )

/**
 * _list_node_unsafe_nextp gets next(of type void*) from n(usually void*)
 * @param  n ptr to a list_node_t
 * @return   ptr to next of n, of type void**
 */
#define _list_node_unsafe_nextp(n)                                             \
  ( (void**)((void*)(n) + sizeof(void*)) )

/**
 * _list_node_unsafe_datap gets data(of type void*) from n(usually void*)
 * @param  n ptr to a list_node_t
 * @return   ptr to data of n, of type void*
 */
#define _list_node_unsafe_datap(n)                                             \
  ( (void*)((void*)(n) + 2 * sizeof(void*)) )

/**
 * list_node_prev gets prev of a list_node_t
 * @param  n ptr to a list_node_t
 * @return   prev of n
 */
#define list_node_prev(n)                                                      \
  ( (n)->prev )

/**
 * list_node_next gets next of a list_node_t
 * @param  n ptr to a list_node_t
 * @return   next of n
 */
#define list_node_next(n)                                                      \
  ( (n)->next )

/**
 * list_node_data gets ata of a list_node_t
 * @param  n ptr to a list_node_t
 * @return   data of n
 */
#define list_node_data(n)                                                      \
  ( (n)->data )

/**
 * list_size returns size of a list_t
 * @param  l ptr to a list_t
 * @return   size of l
 */
#define list_size(l)                                                           \
  ( (l)->size )

/**
 * list_head returns head of a list_t
 * @param  l ptr to a list_t
 * @return   head of l
 */
#define list_head(l)                                                           \
  ( (l)->head )

/**
 * list_tail returns tail of a list_t
 * @param  l ptr to a list_t
 * @return   tail of l
 */
#define list_tail(l)                                                           \
  ( (l)->tail )

/**
 * list_nodesz returns node size of a list_t
 * @param  l ptr to a list_t
 * @return   node size of this list_t
 */
#define list_nodesz(l)                                                         \
  ( sizeof(*(l)->head) )

/**
 * list_is_index_valid checks the idx is valid or not
 * @param  l   ptr to an list_t
 * @param  idx idx to be checked
 * @return     1 if valid else 0
 */
#define list_is_index_valid(l, idx)                                            \
  ( 0 <= (idx) && (idx) < (l)->size ? 1 : 0 )

/**
 * list_safe_index will crash the program if idx exceeds the size
 * @param  l   ptr to a list_t
 * @param  idx idx to be queried
 * @return     idx if safe else crash the program
 */
#define list_safe_index(l, idx)                                                \
  ( list_is_index_valid((l), (idx)) ? (idx) : (assert(0), 0) )

/**
 * list_getn gets the idx-th node
 * @param  l   ptr to an list_t
 * @param  idx index to be queried, must be valid
 * @param  n   place to put ptr to the idx-th node, must be of type list_node_t**
 */
#define list_getn(l, idx, n)                                                   \
  do {                                                                         \
    if (!list_is_index_valid((l), (idx))) { assert(0); }                       \
    *(n) = (l)->head;                                                          \
    for(                                                                       \
      int _i_d_x = 0;                                                          \
      _i_d_x != (idx);                                                         \
      _i_d_x ++, *(n) = *_list_node_unsafe_nextp(*(n)));                       \
  } while(0)

/**
 * list_setn sets the idx-th node
 * @param  l   ptr to an list_t
 * @param  idx index to be queried, must be valid
 * @param  n   ptr to the new node
 */
#define list_setn(l, idx, n)                                                   \
  do {                                                                         \
    if (!list_is_index_valid((l), (idx))) { assert(0); }                       \
    list_remove((l), (idx));                                                   \
    list_insertn((l), (idx), (n));                                             \
  } while(0)

/**
 * list_set sets the idx-th node
 * @param  l   ptr to an list_t
 * @param  idx index to be queried, must be valid
 * @param  x   new value of idx-th node
 */
#define list_set(l, idx, x)                                                    \
  do {                                                                         \
    if (!list_is_index_valid((l), (idx))) { assert(0); }                      \
    list_remove((l), (idx));                                                   \
    list_insert((l), (idx), (x));                                              \
  } while(0)

/**
 * list_appendn appends a node to l
 * @param  l ptr to an list_t
 * @param  n ptr to the node to be appended
 * @return   last index
 */
#define list_appendn(l, n)                                                     \
  ( (l)->size == 0                                                             \
    ? ((l)->tail = (l)->head = (n),                                            \
       (n)->next = (n)->prev = (n),                                            \
       (l)->size ++)                                                           \
    : ((n)->next = (l)->head, (n)->prev = (l)->tail,                           \
       (l)->tail->next = (n), (l)->head->prev = (n),                           \
       (l)->tail = (n), (l)->size ++) )

/**
 * list_append appends a value x to l
 * @param  l ptr to a list_t
 * @param  x value to be appended
 * @return   last index
 */
#define list_append(l, x)                                                      \
  ( _list_unsafe_appendn(                                                      \
      list_unstruct(l),                                                        \
      _list_node_make(list_nodesz(l), &(x), sizeof(x))) )

/**
 * list_prependn prepends a node to l
 * @param  l ptr to an list_t
 * @param  n ptr to the node to be prepended
 * @return   last index
 */
#define list_prependn(l, n)                                                    \
  ( (l)->size == 0                                                             \
    ? ((l)->tail = (l)->head = (n),                                            \
       (n)->next = (n)->prev = (n),                                            \
       (l)->size ++, (0))                                                      \
    : ((n)->next = (l)->head, (n)->prev = (l)->tail,                           \
       (l)->tail->next = (n), (l)->head->prev = (n),                           \
       (l)->head = (n), (l)->size ++, (0)) )

/**
 * list_prepend prepends a value x to l
 * @param  l ptr to a list_t
 * @param  x value to be prepended
 * @return   last index
 */
#define list_prepend(l, x)                                                     \
  ( _list_unsafe_prependn(                                                     \
      list_unstruct(l),                                                        \
      _list_node_make(list_nodesz(l), &(x), sizeof(x))) )

/**
 * list_insertn inserts a node to l at index idx
 * @param  l   ptr to an list_t
 * @param  idx index to be inserted
 * @param  n   ptr to the node to be inserted
 * @return     inserted index
 */
#define list_insertn(l, idx, n)                                                \
  ( _list_unsafe_insertn(list_unstruct(l), (idx), (n)) )

/**
 * list_insert inserts a value x to l at index idx
 * @param  l   ptr to a list_t
 * @param  idx index to be inserted
 * @param  x   value to be inserted
 * @return     inserted index
 */
#define list_insert(l, idx, x)                                                 \
  ( _list_unsafe_insertn(                                                      \
      list_unstruct(l),                                                        \
      (idx),                                                                   \
      _list_node_make(list_nodesz(l), &(x), sizeof(x))) )

/**
 * list_remove removes node at index idx
 * @param  l   ptr to a list_t
 * @param  idx idx to be removed, if idx is invalid, nothing happended
 */
#define list_remove(l, idx)                                                    \
  do {                                                                         \
    if (!list_is_index_valid((l), (idx))) { break; }                           \
    if ((l)->size == 1) {                                                      \
      list_node_deinit((l)->head);                                             \
      list_init(l);                                                            \
      break;                                                                   \
    }                                                                          \
    void *n = (l)->head, *prev, *next;                                         \
    for (int i = 0; i != (idx); i ++) { n = *_list_node_unsafe_nextp(n); }     \
    next = *_list_node_unsafe_nextp(n);                                        \
    prev = *_list_node_unsafe_prevp(n);                                        \
    *_list_node_unsafe_nextp(prev) = next;                                     \
    *_list_node_unsafe_prevp(next) = prev;                                     \
    if (idx == 0) { (l)->head = next; }                                        \
    else if(idx == (l)->size - 1) { (l)->tail = prev; }                        \
    list_node_deinit(n);                                                       \
    (l)->size --;                                                              \
  } while(0)

/**
 * list_swap swaps value in idx1 and idx2
 * @param  l    ptr to a list_t
 * @param  idx1 one index
 * @param  idx2 the other index
 * @return      0 if succeeded else -1
 */
#define list_swap(l, idx1, idx2)                                               \
  ( _list_swap(                                                                \
      list_unstruct(l),                                                        \
      list_safe_index((l), (idx1)),                                            \
      list_safe_index((l), (idx2))) )

/**
 * list_reverse reverses a list_t
 * @param  l ptr to a list_t
 */
#define list_reverse(l)                                                        \
  do {                                                                         \
    if ((l)->size == 0 || (l)->size == 1) { break; }                           \
    void *old_head = (l)->head->next, *ptr;                                    \
    void *new_head = (l)->head, *new_tail = (l)->head;                         \
    (l)->head->next = (l)->head->prev = new_head;                              \
    while(old_head != (l)->head) {                                             \
      ptr = old_head;                                                          \
      old_head = *_list_node_unsafe_nextp(old_head);                           \
      *_list_node_unsafe_prevp(ptr) = new_tail;                                \
      *_list_node_unsafe_nextp(ptr) = new_head;                                \
      *_list_node_unsafe_nextp(new_tail) = ptr;                                \
      *_list_node_unsafe_prevp(new_head) = ptr;                                \
      new_head = ptr;                                                          \
    }                                                                          \
    (l)->head = new_head;                                                      \
    (l)->tail = new_tail;                                                      \
  } while(0)

/**
 * list_foreach_n n indicates node
 * @param  l ptr to a list_t
 * @param  n ptr to node iterator
 */
#define list_foreach_n(l, n)                                                   \
  for (                                                                        \
    int _i_d_x = 0;                                                            \
    (_i_d_x) < (l)->size &&                                                    \
      (_i_d_x == 0 ? ((n) = (l)->head) : ((n) = (n)->next), 1);                \
    (_i_d_x) ++ )

/**
 * list_node_deinit frees a node
 * @param  n ptr to the node to be freed
 */
#define list_node_deinit(n)                                                    \
  do { free(n); } while(0)

/**
 * list_deinit destroys a list_t
 * @param  l ptr to a list_t
 */
#define list_deinit(l)                                                         \
  do {                                                                         \
    void* p = (l)->head;                                                       \
    for(int i = 0; i < (l)->size; i ++) {                                      \
      p = *_list_node_unsafe_nextp(p);                                         \
      list_node_deinit(*_list_node_unsafe_prevp(p));                           \
    }                                                                          \
    list_init(l);                                                              \
  } while(0)

void* _list_node_make(int nodesz, void* x, int xsize);
int   _list_unsafe_insertn(char** head, char** tail, int* size, int nodesz,
                           int idx, void* node);
int   _list_unsafe_appendn(char** head, char** tail, int* size, int nodesz,
                           void* node);
int   _list_unsafe_prependn(char** head, char** tail, int* size, int nodesz,
                            void* node);
int   _list_swap(char** head, char** tail, int* size, int nodesz,
                 int idx1, int idx2);

#endif
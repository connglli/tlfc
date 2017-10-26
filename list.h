#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * list_template declares a list_t and a list_node_t
 * @param  T                 type to be declared
 * @param  list_alias_t      type name alias of list
 * @param  list_node_alias_t type name alias of list node
 * @return                   a wrapped struct
 */
#define list_template(T, list_alias_t, list_node_alias_t)                      \
	typedef struct list_node_alias_t list_node_alias_t;                          \
	typedef struct list_alias_t      list_alias_t;                               \
	/* list type */                                                              \
	struct list_node_alias_t {                                                   \
		list_node_alias_t *prev, *next;                                            \
		T                  data;                                                   \
	};                                                                           \
	/* list node type */                                                         \
	struct list_alias_t {                                                        \
		list_node_alias_t *head;                                                   \
		list_node_alias_t *tail;                                                   \
		int                size;                                                   \
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
 * [list_node_init initializes a node
 * @param  n ptr to a list_node_t
 * @param  x value to be added
 */
#define list_node_init(n, x)                                                   \
	do { (n)->prev = (n)->next = n; (n)->data = (x); } while(0)

/**
 * [list_init initializes a list
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
 * _list_node_unsafe_next gets next(of type void*) from n(usually void*)
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
 * list_appendn appends a node to l
 * @param  l ptr to an list_t
 * @param  n ptr to the node to be appended
 * @return   idx of this node
 */
#define list_appendn(l, n)                                                     \
	( (l)->size == 0 \
		? ((l)->tail = (l)->head = (n), (l)->size ++)                              \
		: ((n)->next = (l)->head, (n)->prev = (l)->tail,                           \
			 (l)->tail->next = (n), (l)->head->prev = (n),                           \
			 (l)->tail = (n), (l)->size ++) )

// The following commented codes can used when append does not need
// any return value.
//
//
// /**
//  * _list_unsafe_appendn appends a node(usually of type void*) to l
//  * @param  l ptr to an list_t
//  * @param  n ptr to the node to be appended
//  * @return   idx of this node
//  */
// #define _list_unsafe_appendn(l, n)                                             \
// 	( (l)->size == 0                                                             \
// 		? ((l)->tail = (l)->head = (n), (l)->size ++)                              \
// 		: (*_list_node_unsafe_nextp(n) = (l)->head,                                \
// 			 *_list_node_unsafe_prevp(n) = (l)->tail,                                \
// 			 (l)->tail->next = (n), (l)->head->prev = (n),                           \
// 			 (l)->tail = (n), (l)->size ++) )
//
// /**
//  * list_append appends a value x to l
//  * @param  l ptr to a list_t
//  * @param  x value to be appended
//  */
// #define list_append(l, x)                                                      \
// 	do {                                                                         \
// 		void* n = _list_node_make(list_nodesz(l), &(x), sizeof(x));                \
// 		_list_unsafe_appendn((l), n);                                              \
// 	} while(0)

/**
 * list_append appends a value x to l
 * @param  l ptr to a list_t
 * @param  x value to be appended
 * @return   idx of x
 */
#define list_append(l, x)                                                      \
	( _list_unsafe_appendn(                                                      \
			list_unstruct(l),                                                        \
			_list_node_make(list_nodesz(l), &(x), sizeof(x))) )

void* _list_node_make(int nodesz, void* x, int xsize);
int   _list_unsafe_appendn(char** head, char** tail, int* size, int nodesz,
													 void* node);

#endif
#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * array_template declares an array_t
 * @param  T             type to be declared
 * @param  array_alias_t type name alias
 * @return               a wrapped struct
 */
#define array_template(T, array_alias_t)                                       \
	typedef struct array_alias_t {                                               \
		T*  data;                                                                  \
		int size;                                                                  \
		int capacity;                                                              \
	} array_alias_t;

/**
 * array_struct declares an array_t
 * @param  T type to be declared
 * @return   a wrapped struct
 */
#define array_struct(T)                                                        \
	struct { T* data; int size; int capacity; }

/**
 * array_unstruct unstructs the array directly to its inner data
 * @param  a ptr to an array_t
 * @return   inner datas
 */
#define array_unstruct(a)                                                      \
	((char**)(&(a)->data)), (&(a)->size), (&(a)->capacity), (sizeof(*(a)->data))

/**
 * make_array makes an array for any array_t
 * @return an array_t
 */
#define make_array() \
	{ NULL, 0, 0 }

/**
 * array_init initialize an array_t
 * @param  a ptr to an array_t
 */
#define array_init(a)                                                          \
	do { memset((a), 0, sizeof(*(a))); } while(0)

/**
 * _array_is_index_valid checks the idx is valid or not
 * @param  a   ptr to an array_t
 * @param  idx idx to be checked
 * @return     1 if valid else 0
 */
#define _array_is_index_valid(a, idx)                                          \
	( 0 <= (idx) && (idx) < (a)->size ? 1 : 0 )

/**
 * array_safe_index will crash the program if idx exceeds the size
 * @param  a   ptr to an array_t
 * @param  idx idx to be queried
 * @return     idx if safe else crash the program
 */
#define array_safe_index(a, idx)                                               \
	( _array_is_index_valid((a), (idx)) ? (idx) : (assert(0), 0) )

/**
 * array_move safely moves array items from from_idx to to_idx, 
 * from_idx and to_idx must be valid, or the program will be crashed,
 * if from_idx is same as to_idx, 
 *   nothing will happen,
 * if from_idx is less than to_idx, size will be automatically increased, 
 *   and all in them are set to 0
 * or decreased.
 * @param  a        ptr to an array_t
 * @param  from_idx index of start point, must be valid
 * @param  to_idx   index of end point, must be valid
 * @return          0 if succeed else -1
 */
#define array_move(a, from_idx, to_idx)                                        \
	( _array_move(                                                               \
			array_unstruct(a),                                                       \
			array_safe_index((a), (from_idx)),                                       \
			array_safe_index((a), (to_idx))) )

/**
 * array_size returns size of an array_t
 * @param  a ptr to an array_t
 * @return   size of a
 */
#define array_size(a)                                                          \
	( (a)->size )

/**
 * array_capacity returns capacity of an array_t
 * @param  a ptr to an array_t
 * @return   capacity of a
 */
#define array_capacity(a)                                                      \
	( (a)->capacity )

/**
 * array_get safely gets the idx-th item
 * @param  a   ptr to an array_t
 * @param  idx idx to be quried, must be valid
 * @return     idx-th item if safe else crash the program
 */
#define array_get(a, idx)                                                      \
	( (a)->data[array_safe_index((a), (idx))] )

/**
 * array_set safely sets the idx-th item
 * @param  a   ptr to an array_t
 * @param  idx idx to be quried, must be valid
 * @param  x   item to be set
 * @return     idx if succeeded else crash the program
 */
#define array_set(a, idx, x)                                                   \
	( (a)->data[array_safe_index((a), (idx))] = (x), (idx) )

/**
 * array_append appends an item to array
 * @param  a ptr to an array_t
 * @param  x item to be appended
 * @return   size of the array
 */
#define array_append(a, x)                                                     \
	( (_array_expand(array_unstruct(a)) == 0)                                    \
		? ((a)->data[(a)->size ++] = (x), (a)->size)                               \
		: (a)->size )

/**
 * array_insert safely insert an item to index idx, and all items from idx
 * are moved to its next position
 * @param  a   ptr to an array_t
 * @param  idx idx to be inserted, must be valid
 * @param  x   item to be inserted
 * @return     idx if succeeded else -1
 */
#define array_insert(a, idx, x)                                                \
	( ((idx) == (a)->size - 1)                                                   \
		? ((_array_expand(array_unstruct(a)) != 0)                                 \
			? (-1)                                                                   \
			: ((a)->data[(idx) + 1] = (a)->data[(idx)],                              \
				 (a)->data[(idx)] = (x),                                               \
				 (a)->size ++,                                                         \
				 (idx)))                                                               \
		: (array_move((a), (idx), (idx) + 1), array_set((a), (idx), (x)), (idx)) )

/**
 * array_prepend prepends an item to array
 * @param  a ptr to an array_t
 * @param  x item to be appended
 * @return   0 if succeeded else -1
 */
#define array_prepend(a, x)                                                    \
	( ((a)->size == 0)                                                          \
		? ((_array_expand(array_unstruct(a)) == 0)                                 \
			? ((a)->data[(a)->size ++] = (x), (0))                                   \
			: (-1))                                                                  \
		: (array_insert((a), 0, (x))))

/**
 * array_remove removes item at index idx
 * @param  a   ptr to an array_t
 * @param  idx idx to be removed, if idx is invalid, nothing happended
 */
#define array_remove(a, idx)                                                   \
	do {                                                                         \
		if ((idx) >= (a)->size) {                                                  \
			break;                                                                   \
		} else if ((idx) == (a)->size - 1) {                                       \
			(a)->size --;                                                            \
		} else {                                                                   \
			array_move((a), (idx) + 1, (idx));                                       \
		}                                                                          \
	} while(0)

/**
 * array_swap swaps idx1 and idx2
 * @param  a    ptr to an array_t
 * @param  idx1 index of one item
 * @param  idx2 index of the other item
 */
#define array_swap(a, idx1, idx2)                                              \
	do {                                                                         \
		_array_swap(                                                               \
			array_unstruct(a),                                                       \
			array_safe_index(a, idx1),                                               \
			array_safe_index(a, idx2));                                              \
	} while(0)

/**
 * array_reverse reverses an array_t
 * @param  a ptr to an array_t
 */
#define array_reverse(a)                                                       \
	do { _array_reverse(array_unstruct(a)); } while(0)

/**
 * array_foreach_i i indicates index
 * @param  a ptr to an array_t
 * @param  i index iterator
 */
#define array_foreach_i(a, i)                                                  \
	for((i) = 0; (i) < (a)->size; (i) ++)

/**
 * array_foreach_v v indicates value
 * @param  a ptr to an array_t
 * @param  v value iterator
 */
#define array_foreach_v(a, v)                                                  \
	for(                                                                         \
		int _i_d_x = 0;                                                            \
		(_i_d_x) < (a)->size && ((v) = (a)->data[_i_d_x], 1);                      \
		_i_d_x ++)

/**
 * array_foreach_p p indicates pointer
 * @param  a ptr to an array_t
 * @param  p pointer iterator
 */
#define array_foreach_p(a, p)                                                  \
	for(                                                                         \
		int _i_d_x = 0;                                                            \
		(_i_d_x) < (a)->size && ((p) = &(a)->data[_i_d_x], 1);                     \
		_i_d_x ++)

/**
 * array_map maps from a to ma using f
 * @param  a  ptr to an array_t
 * @param  ma ptr to the mapped array_t
 * @param  f  transformation function
 */
#define array_map(a, ma, f)                                                    \
	do {                                                                         \
		for (int i = 0; i < (a)->size; i ++) {                                     \
			if (_array_is_index_valid((ma), i)) {                                    \
				array_set((ma), i, f((a)->data[i]));                                   \
			} else {                                                                 \
				array_append((ma), f((a)->data[i]));                                   \
			}                                                                        \
		}                                                                          \
	} while(0)

/**
 * array_map maps from a to ma using f
 * @param  a  ptr to an array_t
 * @param  ma ptr to the mapped array_t
 * @param  f  transformation function
 */
#define array_reduce(a, rv, iniv, f)                                           \
	do {                                                                         \
	  *(rv) = (iniv);                                                            \
		for (int i = 0; i < (a)->size; i ++) {                                     \
			*(rv) = f(*(rv), (a)->data[i]);                                          \
		}                                                                          \
	} while(0)

/**
 * array_deinit destroied an array_t
 * @param  a ptr to an array_t
 * @return   0
 */
#define array_deinit(a)                                                        \
	do { free((a)->data); array_init((a)); } while(0)

int _array_expand(char** data, int* size, int* capacity, int itemsz);
int _array_force_expand(char** data, int* size, int* capacity, int itemsz);
int _array_reverse(char** data, int* size, int* capacity, int itemsz);
int _array_swap(char** data, int* size, int* capacity, int itemsz, 
								int idx1, int idx2);
int _array_move(char** data, int* size, int* capacity, int itemsz, 
								int from_idx, int to_idx);

#endif
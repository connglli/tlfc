#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <stdint.h>
#include <assert.h>

#define __cat3(a, b, c) a##b##c

/**
 * BITMAP_NR_BITS_PER_CONTAINER defines number bits of one container
 */
#define BITMAP_NR_BITS_PER_CONTAINER 64

/**
 * [BITMAP_CONTAINER_TYPE uses x to define a bitmap_container_t
 * @param  x number bits of one container
 * @return   
 */
#define BITMAP_CONTAINER_TYPE(x) __cat3(uint, x, _t)

/**
 * BITMAP_GET_NR_CONTAINER gets number of containers via number of bits
 * @param  size number of bits
 * @return      number of container
 */
#define BITMAP_GET_NR_CONTAINER(size)                                          \
  ceil((double)(size) / BITMAP_NR_BITS_PER_CONTAINER)

/**
 * bitmap_container_t represents a bitmap container
 */
typedef BITMAP_CONTAINER_TYPE(BITMAP_NR_BITS_PER_CONTAINER) bitmap_container_t;

/**
 * bitmap_t represents a bitmap
 */
typedef struct bitmap_t {
  bitmap_container_t *containers;
  int                 size;
} bitmap_t;

/**
 * make_bitmap makes a bitmap
 * @param  size size of bits
 * @return      a initialized bitmap
 */
bitmap_t make_bitmap(int size);

/**
 * bitmap_init initializes a bitmap_t
 * @param  bm   bm to be initialized
 * @param  size size of bits of this bitmap
 */
void bitmap_init(bitmap_t *bm, int size);

/**
 * bitmap_size returns size of a bitmap_t
 * @param  bm ptr to a bitmap_t
 * @return    size of bm
 */
#define bitmap_size(bm)                                                        \
  ( (bm)->size )

/**
 * bitmap_is_index_valid checks the idx is valid or not
 * @param  bm   ptr to an bitmap_t
 * @param  idx  idx to be checked
 * @return      1 if valid else 0
 */
#define bitmap_is_index_valid(bm, idx)                                         \
  ( 0 <= (idx) && (idx) < (bm)->size ? 1 : 0 )

/**
 * bitmap_safe_index will crash the program if idx exceeds the size
 * @param  bm   ptr to a bitmap_t
 * @param  idx  idx to be queried
 * @return      idx if safe else crash the program
 */
#define bitmap_safe_index(bm, idx)                                             \
  ( bitmap_is_index_valid((bm), (idx)) ? (idx) : (assert(0), 0) )

/**
 * bitmap_get gets the idx-th bit value of bm
 * @param  bm  ptr to a bitmap 
 * @param  idx index to be queried
 * @return     value of idx-th bit: 0 or 1
 */
int bitmap_get(bitmap_t *bm, int idx);

/**
 * bitmap_set set the idx-th bit to 1
 * @param bm  ptr to a bitmap 
 * @param idx index to be queried
 */
void bitmap_set(bitmap_t *bm, int idx);

/**
 * bitmap_clear clears the idx-th bit(to 0)
 * @param bm  ptr to a bitmap 
 * @param idx index to be queried
 */
void bitmap_clear(bitmap_t *bm, int idx);

/**
 * bitmap_expand_to_cap  expands a bitmap_t to cap
 * @param bm  ptr to a bitmap 
 * @param cap capacity to be expanded
 * @return    0 if succeeded else -1
 */
int bitmap_expand_to_cap(bitmap_t *bm, int cap);

/**
 * bitmap_deinit deinitialzes a bitmap_t
 * @param bm ptr to a bitmap 
 */
void bitmap_deinit(bitmap_t *bm);

#endif
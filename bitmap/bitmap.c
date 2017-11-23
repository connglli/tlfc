#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "bitmap.h"

// BITMAP_CONTAINER_SIZE is the size of bitmap_container_t
#define BITMAP_CONTAINER_SIZE sizeof(bitmap_container_t)

// BITMAP_GET_NR_CONTAINER gets number of containers via number of bits
#define BITMAP_GET_NR_CONTAINER(size)                                          \
  ceil((double)(size) / BITMAP_CONTAINER_SIZE)

// BITMAP_GET_BIT_INFO gets idx-th bit info of bm to (d, r)
// bm  bm  ptr to a bitmap_t
// idx index to be queried
// d   index of container
// r   offset in d-th container
#define BITMAP_GET_BIT_INFO(bm, idx, d, r)                                     \
 do {                                                                          \
  (idx) = bitmap_safe_index((bm), (idx));                                      \
  (d)   = (idx) / BITMAP_CONTAINER_SIZE;                                       \
  (r)   = (idx) % BITMAP_CONTAINER_SIZE;                                       \
} while(0)

/**
 * make_bitmap makes a bitmap
 * @param  size size of bits
 * @return      a initialized bitmap
 */
bitmap_t make_bitmap(int size) {
  bitmap_t bm;
  bitmap_init(&bm, size);
  return bm;
}

/**
 * bitmap_init initializes a bitmap_t
 * @param  bm   bm to be initialized
 * @param  size size of bits of this bitmap
 */
void bitmap_init(bitmap_t *bm, int size) {
  int nr_container = BITMAP_GET_NR_CONTAINER(size);
  bm->containers = malloc(nr_container * BITMAP_CONTAINER_SIZE);
  if (NULL == bm->containers) { assert(0); }
  memset(bm->containers, 0, nr_container * BITMAP_CONTAINER_SIZE);
  bm->size = size;
}

/**
 * bitmap_get gets the idx-th bit value of bm
 * @param  bm  ptr to a bitmap 
 * @param  idx index to be queried
 * @return     value of idx-th bit: 0 or 1
 */
int bitmap_get(bitmap_t *bm, int idx) {
  int d, r = -1;
  BITMAP_GET_BIT_INFO(bm, idx, d, r);
  return (bm->containers[d] >> r) & 1;
}

/**
 * bitmap_set set the idx-th bit to 1
 * @param bm  ptr to a bitmap 
 * @param idx index to be queried
 */
void bitmap_set(bitmap_t *bm, int idx) {
  int d, r = -1;
  BITMAP_GET_BIT_INFO(bm, idx, d, r);
  bm->containers[d] |= (((bitmap_container_t)1) << r);
}

/**
 * bitmap_clear clears the idx-th bit(to 0)
 * @param bm  ptr to a bitmap 
 * @param idx index to be queried
 */
void bitmap_clear(bitmap_t *bm, int idx) {
  int d, r = -1;
  BITMAP_GET_BIT_INFO(bm, idx, d, r);
  bm->containers[d] &= ~(((bitmap_container_t)1) << r);
}

/**
 * bitmap_expand_to_cap expands a bitmap_t to cap
 * @param bm  ptr to a bitmap 
 * @param cap capacity to be expanded
 * @return    0 if succeeded else -1
 */
int bitmap_expand_to_cap(bitmap_t *bm, int cap) {
  if (cap <= bitmap_size(bm)) { return 0; }

  int nr_container     = BITMAP_GET_NR_CONTAINER(cap);
  int old_nr_container = BITMAP_GET_NR_CONTAINER(bm->size);
  
  if (nr_container == old_nr_container) {
    bm->size = cap;
    return 0;
  }

  bitmap_container_t *p = realloc(bm->containers,
                                  nr_container * BITMAP_CONTAINER_SIZE);
  if (NULL == p) { return -1; }
  memset(p + (nr_container - old_nr_container),
         0,
         (nr_container - old_nr_container) * BITMAP_CONTAINER_SIZE);
  bm->containers = p;
  bm->size       = cap;

  return 0;
}

/**
 * bitmap_deinit deinitialzes a bitmap_t
 * @param bm ptr to a bitmap 
 */
void bitmap_deinit(bitmap_t *bm) {
  if (NULL != bm->containers) {
    free(bm->containers);
    bm->containers = NULL;
    bm->size       = 0;
  }
}
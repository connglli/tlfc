#ifndef __BLOOM_FILTER_H__
#define __BLOOM_FILTER_H__

#include "bitmap.h"
#include "hash.h"

/**
 * bloom_filter_t represents standard bloom filter
 */
typedef struct bloom_filter_t {
  /* parameters */
  int       m; // number of bits
  int       n; // number of elements
  int       k; // number of hash functions
  double    e; // error rate

  /* hashes */
  hash_f_t *hashes;

  /* bitmap */
  bitmap_t  bitmap;
} bloom_filter_t;

/**
 * make_bloom_filter makes a bloom_filter_t
 * @param  n number of element would like to contain
 * @param  e error rate
 * @return   a initialized bloom_filter_t
 */
bloom_filter_t make_bloom_filter(int n, double e);

/**
 * bloom_filter_init initializes a bloom_filter_t
 * @param bf ptr to a bloom_filter_t
 * @param n  number of element would like to contain
 * @param e  error rate
 */
void bloom_filter_init(bloom_filter_t *bf, int n, double e);

/**
 * bloom_filter_contain check whether bf contains o
 * @param  bf   ptr to a bloom_filter_t
 * @param  o    ptr to a object to be queried
 * @param  size size of this object o
 * @return      1 if contains else 0
 */
int bloom_filter_contain(bloom_filter_t *bf, void * o, int size);

/**
 * bloom_filter_enter add an object o of size size
 * @param bf   ptr to a bloom_filter_t
 * @param o    ptr to a object to be entered
 * @param size size of this object o
 */
void bloom_filter_enter(bloom_filter_t *bf, void * o, int size);

/**
 * bloom_filter_deinit deinitializes a bloom filter
 * @param bf ptr to a bloom_filter_t
 */
void bloom_filter_deinit(bloom_filter_t *bf);

#endif
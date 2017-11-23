#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "bloom-filter.h"

/**
 * make_bloom_filter makes a bloom_filter_t
 * @param  n number of element would like to contain
 * @param  e error rate
 * @return   a initialized bloom_filter_t
 */
bloom_filter_t make_bloom_filter(int n, double e) {
  bloom_filter_t bf;
  bloom_filter_init(&bf, n, e);
  return bf;
}

/**
 * bloom_filter_init initializes a bloom_filter_t
 * @param bf ptr to a bloom_filter_t
 * @param n  number of element would like to contain
 * @param e  error rate
 */
void bloom_filter_init(bloom_filter_t *bf, int n, double e) {
  static hash_f_t available_hashes[] = {
    hash_rs,  hash_js, hash_pjw, hash_bkdr,   hash_sdbm,
    hash_djb, hash_ap, hash_crc, hash_simple, hash_elf
  };
  static int nr_hashes = sizeof(available_hashes) / sizeof(hash_f_t);
  
  int m = ceil(1.44 * n * log2(1 / e));
  int k = ceil(((double)m / n) * log(2));
  if (k > nr_hashes) { k = nr_hashes; }

  // initialize hashes
  bf->hashes = malloc(k * sizeof(hash_f_t));
  if (NULL == bf->hashes) { assert(0); }

  for (int i = 0; i < k; i ++) {
    bf->hashes[i] = available_hashes[i];
  }

  // initialize bitmap
  bitmap_init(&bf->bitmap, m);

  // initialize parameters
  bf->m = m;
  bf->k = k;
  bf->n = n;
  bf->e = e;
}

/**
 * bloom_filter_contain checks whether bf contains o
 * @param  bf   ptr to a bloom_filter_t
 * @param  o    ptr to a object to be queried
 * @param  size size of this object o
 * @return      1 if contains else 0
 */
int bloom_filter_contain(bloom_filter_t *bf, void *o, int size) {
  bitmap_t *bm     = &bf->bitmap;
  hash_f_t *hashes = bf->hashes;
  int       k      = bf->k;
  int       m      = bf->m;

  int r = 1;

  for (int i = 0; r && (i < k); i ++) {
    r = r && bitmap_get(bm, (hashes[i]((const char*)o, size)) % m);
  }

  return r;
}

/**
 * bloom_filter_enter adds an object o of size size
 * @param bf   ptr to a bloom_filter_t
 * @param o    ptr to a object to be entered
 * @param size size of this object o
 */
void bloom_filter_enter(bloom_filter_t *bf, void *o, int size) {
  bitmap_t *bm     = &bf->bitmap;
  hash_f_t *hashes = bf->hashes;
  int       k      = bf->k;
  int       m      = bf->m;

  for (int i = 0; i < k; i ++) {
    bitmap_set(bm, (hashes[i]((const char*)o, size)) % m);
  }
}

/**
 * bloom_filter_deinit deinitializes a bloom filter
 * @param bf ptr to a bloom_filter_t
 */
void bloom_filter_deinit(bloom_filter_t *bf) {
  if (NULL != bf->hashes) {
    free(bf->hashes);
    bf->hashes = NULL;
  }
  bitmap_deinit(&bf->bitmap);
}
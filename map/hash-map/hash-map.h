#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include <stdlib.h>
#include "hash.h"

#define __cat2(a, b) a##b

/**
 * HASH_MAP_DEFAULT_NR_BUCKETS is the default number of buckets
 */
#define HASH_MAP_DEFAULT_NR_BUCKETS 256

/**
 * hash_map_template declares a map_t and a map_entry_t
 * @param  T           value type saved in this map
 * @param  map_entry_t type name alias of map entry
 * @param  map_t       type name alias of map
 */
#define hash_map_template(T, map_entry_t, map_t)                               \
  typedef struct map_entry_t map_entry_t;                                      \
  typedef struct map_t       map_t;                                            \
  /* map entry type, we name it map_entry_t in the folowing */                 \
  struct map_entry_t {                                                         \
    map_entry_t *bnext; /* next in bucket */                                   \
    map_entry_t *onext; /* next in some order */                               \
    char        *key;                                                          \
    T            value;                                                        \
  };                                                                           \
  /* map type, we name it map_t in the folowing */                             \
  struct map_t {                                                               \
    hash_f_t     hash;                                                         \
    int          size;                                                         \
    map_entry_t *buckets[HASH_MAP_DEFAULT_NR_BUCKETS];                         \
  };

/**
 * hash_map_struct declares an hash_map_t
 * @param  ET type to be declared as map entry type
 * @return    a wrapped struct
 */
#define hash_map_struct(ET)                                                    \
  struct {                                                                     \
    hash_f_t  hash;                                                            \
    int       size;                                                            \
    ET       *buckets[HASH_MAP_DEFAULT_NR_BUCKETS];                            \
  }

/**
 * hash_map_unstruct unstructs the hash_map_t directly to its inner data
 * @param  hm ptr to a hash_map_t
 * @return    inner datas
 */
#define hash_map_unstruct(hm)                                                  \
  ( (hm)->hash ),                                                              \
  ( &(hm)->size ),                                                             \
  ( (void**)((hm)->buckets) )

/**
 * _map_entry_unsafe_bnextp gets ptr to bnext(in type void**) from e(mostly void*)
 * @param  e ptr to a map_entry_t
 * @return   ptr to bnext of e, in type void**
 */
#define _hash_map_entry_unsafe_bnextp(e)                                       \
  ( (void**)(e) )

/**
 * _map_entry_unsafe_onextp gets ptr to onext(of type void**) from e(usually void*)
 * @param  e ptr to a map_entry_t
 * @return   ptr to onext of e, of type void**
 */
#define _hash_map_entry_unsafe_onextp(e)                                       \
  ( (void**)((void*)(e) + sizeof(void*)) )

/**
 * _map_entry_unsafe_keyp gets ptr to key(of type char**) from e(usually void*)
 * @param  e ptr to a map_entry_t
 * @return   ptr to key of e, of type char**
 */
#define _hash_map_entry_unsafe_keyp(e)                                         \
  ( (char**)((void*)(e) + 2 * sizeof(void*)) )

/**
 * _map_entry_unsafe_valuep gets ptr to value(of type void*) from e(usually void*)
 * @param  e ptr to a map_entry_t
 * @return   ptr to data of n, of type void*
 */
#define _hash_map_entry_unsafe_valuep(e)                                       \
  ( (void*)((void*)(e) + 3 * sizeof(void*)) )

/**
 * hash_map_entry_key gets key of a hash_map_entry_t
 * @param  e ptr to a hash_map_entry_t
 * @return   key of a e
 */
#define hash_map_entry_key(e) \
  ( (e)->key )

/**
 * hash_map_entry_value gets value of a hash_map_entry_t
 * @param  e ptr to a hash_map_entry_t
 * @return   value of a e
 */
#define hash_map_entry_value(e) \
  ( (e)->value )

/**
 * hash_map_init initializes a hash_map_t
 * @param  hm ptr to a hash_map_t
 */
#define hash_map_init(hm)                                                      \
  do {                                                                         \
    for (int i = 0; i < HASH_MAP_DEFAULT_NR_BUCKETS; i ++) {                   \
      (hm)->buckets[i] = NULL;                                                 \
    }                                                                          \
    (hm)->size = 0;                                                            \
    (hm)->hash = hash_simple;                                                  \
  } while(0)

/**
 * hash_map_size returns size of a hash_map_t
 * @param  hm ptr to a hash_map_t
 * @return    size of hm
 */
#define hash_map_size(hm)                                                      \
  ( (hm)->size )

/**
 * hash_map_entrysz returns size of a hash_map_entry_t of a hash_map_t
 * @param  hm ptr to a hash_map_t
 * @return    size of the map entry of hm
 */
#define hash_map_entrysz(hm)                                                   \
  ( sizeof(**(hm)->buckets) )

/**
 * hash_map_load_factor returns load factor of a hash_map_t
 * @param  hm ptr to a hash_map_t
 * @return    load factor of hm
 */
#define hash_map_load_factor(hm)                                               \
  ( (double)(hm)->size / HASH_MAP_DEFAULT_NR_BUCKETS )

/**
 * hash_map_hash returns hash function of a hash_map_t
 * @param  hm ptr to a hash_map_t
 * @return    hash function of hm
 */
#define hash_map_hash(hm) \
  ( (hm)->hash )

/**
 * hash_map_set_hash sets hash function of a hash_map_t
 * @param  hm ptr to a hash_map_t
 * @param  hf hash function to be set
 */
#define hash_map_set_hash(hm, hf) \
  do { (hm)->hash = (hf); } while(0)

/**
 * hash_map_contain checks whether a hash_map_t contains k
 * @param  hm ptr to a hash_map_t
 * @param  k  key to be queried
 * @return    1 if find else 0
 */
#define hash_map_contain(hm, k)                                                \
  ( _hash_map_contain(hash_map_unstruct(hm), k) )

/**
 * hash_map_set sets/updates (k, v)
 * @param  hm ptr to a hash_map_t
 * @param  k  key to be set/updated
 * @param  v  value to be set/updated
 * @return    0 if succeeded else -1
 */
#define hash_map_set(hm, k, v)                                                 \
  ( _hash_map_set(hash_map_unstruct(hm), hash_map_entrysz(hm),                 \
                  k, &v, sizeof(v)) )

/**
 * hash_map_get saves the value of key k to pv
 * @param  hm ptr to a hash_map_t
 * @param  k  key to be queried
 * @param  pv ptr to the value
 * @return    0 if succeeded else -1
 */
#define hash_map_get(hm, k, pv)                                                \
  ( _hash_map_get(hash_map_unstruct(hm),                                       \
                  k, (void*)pv, sizeof(*pv)) )

/**
 * hash_map_remove removes the entry shipped with key k
 * @param  hm ptr to a hash_map_t
 * @param  k  key to be queried
 * @return    always return 1
 */
#define hash_map_remove(hm, k)                                                 \
  ( _hash_map_remove(hash_map_unstruct(hm), k) )

/**
 * hash_map_deinit deinitializes a hash_map_t
 * @param  hm ptr to a hash_map_t
 */
#define hash_map_deinit(hm)                                                    \
  do {                                                                         \
    for (int i = 0; i < HASH_MAP_DEFAULT_NR_BUCKETS; i ++) {                   \
      void *p;                                                                 \
      while((p = (void*)((hm)->buckets[i])) != NULL) {                         \
        (hm)->buckets[i] = *_hash_map_entry_unsafe_bnextp(p);                  \
        free(*_hash_map_entry_unsafe_keyp(p));                                 \
        free(p);                                                               \
      }                                                                        \
      (hm)->buckets[i] = NULL;                                                 \
    }                                                                          \
  } while(0)

/**
 * hash_map_foreach_e e indicates entry
 * @param  hm ptr to a hash_map_t
 * @param  e  ptr to a hash_map_entry_t
 */
#define hash_map_foreach_e(hm, e) \
  for (int _i_d_x = 0; _i_d_x < HASH_MAP_DEFAULT_NR_BUCKETS; _i_d_x ++)     \
    for (e = (hm)->buckets[_i_d_x]; e; e = *_hash_map_entry_unsafe_bnextp(e))

int _hash_map_contain(hash_f_t hash, int *size, void** buckets,
                      const char *k);
int _hash_map_set(hash_f_t hash, int *size, void** buckets, int entrysz,
                  const char *k, void *v, int vsize);
int _hash_map_get(hash_f_t hash, int *size, void** buckets,
                  const char *k, void *v, int vsize);
int _hash_map_remove(hash_f_t hash, int *size, void** buckets,
                     const char *k);

#endif
#include <stdlib.h>
#include <string.h>
#include "hash-map.h"

#define HASH_MAP_GET_BUCKET_INDEX(hash, k) \
  (hash)((k), strlen(k)) % HASH_MAP_DEFAULT_NR_BUCKETS

/**
 * strdup duplicates a string from s
 * @param  s string you want to be duplicated
 * @return   a duplicate of s, remember to free it
 */
char* strdup(const char* s) {
  if (NULL == s) { return NULL; }
  size_t l = strlen(s);
  char *ds = (char*)malloc((l + 1) * sizeof(char));
  strcpy(ds, s); ds[l] = '\0';
  return ds;
}

/**
 * _hash_map_entry_make allocates and then initializes an entry
 * @param entrysz size of an entry
 * @param k       key of this entry
 * @param v       value of this entry
 * @param vsize   size of value
 */
static
void* _hash_map_entry_make(int entrysz, const char *k, void *v, int vsize) {
  void *entry = malloc(entrysz);
  if (entry == NULL) { return entry; }

  *_hash_map_entry_unsafe_bnextp(entry) = NULL;
  *_hash_map_entry_unsafe_onextp(entry) = NULL;
  *_hash_map_entry_unsafe_keyp(entry)   = strdup(k);
  memcpy(_hash_map_entry_unsafe_valuep(entry), v, vsize);

  return entry;
}

/**
 * _hash_map_find_entry finds an hash_map_entry_t
 * @param hash    hash function of a hash_map_t
 * @param size    size of a hash_map_t
 * @param buckets buckets of a hash_map_t
 * @param k       key to be queried
 * @return        ptr(void*) to the entry shipped with key k, or NULL
 */
static
void* _hash_map_find_entry(hash_f_t hash, int *size, void** buckets,
                           const char *k) {
  int idx = HASH_MAP_GET_BUCKET_INDEX(hash, k);

  for (void *p = buckets[idx]; p; p = *_hash_map_entry_unsafe_bnextp(p)) {
    if (strcmp(*_hash_map_entry_unsafe_keyp(p), k) == 0) {
      return p;
    }
  }

  return NULL;
}

/**
 * _hash_map_contain checks whether a hash_map_t contains k
 * @param hash    hash function of a hash_map_t
 * @param size    size of a hash_map_t
 * @param buckets buckets of a hash_map_t
 * @param k       key to be queried
 * @return        1 if find else 0
 */
int _hash_map_contain(hash_f_t hash, int *size, void** buckets,
                      const char *k) {
  return _hash_map_find_entry(hash, size, buckets, k) != NULL;
}

/**
 * _hash_map_set sets/updates (k, v)
 * @param  hash    hash function of a hash_map_t
 * @param  size    size of a hash_map_t
 * @param  buckets buckets of a hash_map_t
 * @param  entrysz size of a hash_map_entry_t
 * @param  k       key to be set/updated
 * @param  v       value to be set/updated
 * @param  vsize   size of the value
 * @return         0 if succeeded else -1
 */
int _hash_map_set(hash_f_t hash, int *size, void** buckets, int entrysz,
                  const char *k, void *v, int vsize) {
  int   idx   = HASH_MAP_GET_BUCKET_INDEX(hash, k);
  void* entry = _hash_map_find_entry(hash, size, buckets, k);

  // update
  if (NULL != entry) {
    memcpy(_hash_map_entry_unsafe_valuep(entry), v, vsize);
    return 0;
  }

  // add
  entry = _hash_map_entry_make(entrysz, k, v, vsize);
  if (NULL == entry) { return -1; }

  if (NULL == buckets[idx]) {
    buckets[idx] = entry;
  } else {
    *_hash_map_entry_unsafe_bnextp(entry) = buckets[idx];
    buckets[idx] = entry;
  }

  (*size) ++;

  return 0;
}

/**
 * _hash_map_get gets the value v of key k
 * @param  hash    hash function of a hash_map_t
 * @param  size    size of a hash_map_t
 * @param  buckets buckets of a hash_map_t
 * @param  k       key to be queried
 * @param  v       ptr to value
 * @param  vsize   size of the value
 * @return         0 if succeeded else -1
 */
int _hash_map_get(hash_f_t hash, int *size, void** buckets,
                  const char *k, void *v, int vsize) {
  void* entry = _hash_map_find_entry(hash, size, buckets, k);

  if (NULL != entry) {
    memcpy(v, _hash_map_entry_unsafe_valuep(entry), vsize);
    return 0;
  }

  return -1;
}

/**
 * _hash_map_remove removes the entry shipped with key k
 * @param  hash    hash function of a hash_map_t
 * @param  size    size of a hash_map_t
 * @param  buckets buckets of a hash_map_t
 * @param  k       key to be queried
 * @return         always return 1
 */
int _hash_map_remove(hash_f_t hash, int *size, void** buckets,
                     const char *k) {
  int idx = HASH_MAP_GET_BUCKET_INDEX(hash, k);

  void *curr = buckets[idx], *prev = NULL;

  while(curr) {
    if (strcmp(*_hash_map_entry_unsafe_keyp(curr), k) == 0) {
      if (NULL != prev) {
        *_hash_map_entry_unsafe_bnextp(prev) =
           *_hash_map_entry_unsafe_bnextp(curr);
      } else {
        buckets[idx] =
          *_hash_map_entry_unsafe_bnextp(curr);
      }

      free(*_hash_map_entry_unsafe_keyp(curr));
      free(curr);

      (*size) --;

      break;
    }

    prev = curr;
    curr = *_hash_map_entry_unsafe_bnextp(curr);
  }

  return 1;
}
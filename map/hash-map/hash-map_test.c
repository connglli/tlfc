#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "hash-map.h"

typedef struct test_value_t {
  double value;
} test_value_t;

hash_map_template(test_value_t,
                  test_hash_map_entry_t,
                  test_hash_map_t);

int main() {
  test_hash_map_t hm;
  char            key[16];

  hash_map_init(&hm);
  assert(0 == hash_map_size(&hm));
  assert(0 == hash_map_load_factor(&hm));

  for (int i = 0; i < HASH_MAP_DEFAULT_NR_BUCKETS; i ++) {
    assert(NULL == hm.buckets[i]);
  }

  for (int i = 0; i < 100; i ++) {
    sprintf(key, "%d", i);
    test_value_t value = { (double)i / 100 };

    assert(hash_map_contain(&hm, key) == 0);

    hash_map_set(&hm, key, value);
    assert(hash_map_contain(&hm, key) == 1);

    test_value_t v;
    hash_map_get(&hm, key, &v);
    assert(fabs(v.value - value.value) < 1e-6);
  }

  assert(100 == hash_map_size(&hm));

  test_hash_map_entry_t* e;
  hash_map_foreach_e(&hm, e) {
    int k; sscanf(e->key, "%d", &k);
    assert(fabs((double)k / 100 - e->value.value) < 1e-6);
  }

  for (int i = 0; i < 100; i ++) {
    sprintf(key, "%d", i);

    assert(hash_map_contain(&hm, key) == 1);

    hash_map_remove(&hm, key);
    assert(hash_map_contain(&hm, key) == 0);
  }

  assert(hash_map_size(&hm) == 0);

  hash_map_deinit(&hm);

  return 0;
}
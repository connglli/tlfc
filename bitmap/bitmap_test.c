#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "bitmap.h"

#define BM_SIZE     1000
#define NEW_BM_SIZE BM_SIZE * 2

int main() {
  srand(time(NULL));

  bitmap_t bm = make_bitmap(BM_SIZE);
  assert(bitmap_size(&bm) == BM_SIZE);

  for (int i = 0; i < BM_SIZE; i ++) {
    assert(bitmap_get(&bm, i) == 0);
  }

  int count = (int)((double)rand() / RAND_MAX * BM_SIZE);
  for (int i = 0; i < count; i ++) {
    int idx = (int)(rand() / RAND_MAX * BM_SIZE);

    bitmap_set(&bm, idx);
    assert(bitmap_get(&bm, idx) == 1);

    bitmap_clear(&bm, idx);
    assert(bitmap_get(&bm, idx) == 0);
  }


  for (int i = 0; i < count; i ++) {
    bitmap_set(&bm, i);
    assert(bitmap_get(&bm, i) == 1);
  }

  bitmap_expand_to_cap(&bm, NEW_BM_SIZE);
  assert(bitmap_size(&bm) == NEW_BM_SIZE);

  for (int i = 0; i < NEW_BM_SIZE; i ++) {
    if (i < count) {
      assert(bitmap_get(&bm, i) == 1);
    } else {
      assert(bitmap_get(&bm, i) == 0);
    }
  }

  for (int i = 0; i < NEW_BM_SIZE; i ++) {
      bitmap_set(&bm, i);
  }

  int nr_containers = 
    ceil((double)(bitmap_size(&bm) / BITMAP_NR_BITS_PER_CONTAINER));
  for (int i = 0; i < nr_containers - 1; i ++) {
    assert((bitmap_container_t)-1 == bm.containers[i]);
  }

  bitmap_deinit(&bm);

  return 0;
}
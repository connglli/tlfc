#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloom-filter.h"

#define MAX_ELEMENT 1000
#define ERROR_RATE  0.01

typedef struct test_struct {
  char buf[16];
  int  x;
} test_struct;

int main() {
  bloom_filter_t bf = make_bloom_filter(MAX_ELEMENT, ERROR_RATE);
  assert(bf.n == MAX_ELEMENT);

  int          count    = (int)((double)rand() / RAND_MAX * MAX_ELEMENT);
  test_struct *t_buf = (test_struct*)malloc(count * sizeof(int));
  for (int i = 0; i < count; i ++) {
    int idx = (int)(rand() / RAND_MAX * MAX_ELEMENT);

    sprintf(t_buf[i].buf, "test");
    sprintf(t_buf[i].buf + strlen(t_buf[i].buf) + 1, "\n%d", idx);
    t_buf[i].x = idx;

    bloom_filter_enter(&bf, (void*)&t_buf[i], sizeof(test_struct));
  }

  for (int i = 0; i < count; i ++) {
    assert(bloom_filter_contain(&bf, (void*)&t_buf[i], sizeof(test_struct)) == 1);
  }

  bloom_filter_deinit(&bf);

  return 0;
}
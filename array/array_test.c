#include <stdio.h>
#include "array.h"

array_template(long, array_long_t);
typedef array_struct(int) array_int_t;

long f(int i) {
  return (long)(2 * i);
}

int ff(int i, int j) {
  return j + i;
}

void test_array_int_t() {
  array_int_t l;
  array_init(&l);

  for(int i = 0; i < 10; i ++) {
    array_append(&l, i);
  }

  int s = array_size(&l);
  for(int i = 0; i < s; i ++) {
    assert(array_get(&l, i) == i);
  }

  s = array_size(&l);
  for(int i = 0; i < s; i ++) {
    assert(*array_getp(&l, i) == i);
  }

  array_reverse(&l);
  for(int i = 0; i < s; i ++) {
    assert(array_get(&l, i) == s - i - 1);
  }
  array_reverse(&l);

  array_append(&l, 100);
  assert(array_get(&l, s) == 100);

  array_set(&l, s, 101);
  assert(array_get(&l, s) == 101);

  array_append(&l, 102);
  array_swap(&l, s, s + 1);
  assert(array_get(&l, s) == 102);
  assert(array_get(&l, s + 1) == 101);

  array_insert(&l, s + 1, 100);
  assert(array_get(&l, s + 1) == 100);
  assert(array_get(&l, s + 2) == 101);
  assert(array_size(&l) == s + 3);

  array_remove(&l, s + 2);
  assert(array_size(&l) == s + 2);

  array_remove(&l, s + 1);
  assert(array_size(&l) == s + 1);

  array_remove(&l, s);
  assert(array_size(&l) == s);

  array_insert(&l, 0, 1234);
  assert(array_get(&l, 0) == 1234);
  for(int i = 1; i <= s; i ++) {
    assert(array_get(&l, i) == i - 1);
  }

  array_remove(&l, 0);
  assert(array_size(&l) == s);

  array_move(&l, 0, 2);
  for(int i = 2; i < array_size(&l); i ++) {
    if (i == 0 || i == 1) {
      assert(array_get(&l, i) == 0);
    }
    else {
      assert(array_get(&l, i) == i - 2);
    }
  }

  array_move(&l, 2, 0);
  for(int i = 0; i < s; i ++) {
    assert(array_get(&l, i) == i);
  }

  assert(array_size(&l) == 10);
  assert(array_capacity(&l) == 16);
  array_shrink_to_fit(&l);
  assert(array_size(&l) == 10);
  assert(array_capacity(&l) == 10);

  array_expand_to_cap(&l, 7);
  assert(array_size(&l) == 10);
  assert(array_capacity(&l) == 10);

  array_expand_to_cap(&l, 10);
  assert(array_size(&l) == 10);
  assert(array_capacity(&l) == 10);

  array_expand_to_cap(&l, 12);
  assert(array_size(&l) == 10);
  assert(array_capacity(&l) == 12);

  int v, i = 0;
  array_foreach_v(&l, v) {
    assert(v == i ++);
  }

  array_long_t ml = make_array();

  array_map(&l, &ml, f);
  for (int i = 0; i < array_size(&ml); i ++) {
    assert(array_get(&ml, i) == i * 2);
  }

  int sum;
  array_reduce(&l, &sum, 0, ff);
  assert(sum == 45);

  array_prepend(&l, 1234);
  assert(array_get(&l, 0) == 1234);
  for (int i = 1; i < array_size(&l); i ++) {
    assert(array_get(&l, i) == i - 1);
  }

  array_deinit(&l);
}

int main() {
  test_array_int_t();
}
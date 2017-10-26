#include <stdio.h>
#include <stdlib.h>
#include "list.h"

list_template(int, list_int_t, list_node_int_t);

int main() {
  list_int_t l;
  list_init(&l);
  list_node_int_t *n, *p;
  void* v;
  int i, s;

  for(int i = 0; i < 5; i ++) {
    list_append(&l, i);
  }

  for(int i = 5; i < 10; i ++) {
    n = list_node_make(list_node_int_t, i);
    assert(n->next == n && n->prev == n && n->data == i);
    assert(list_node_next(n) == n && 
      list_node_prev(n) == n && 
      list_node_data(n) == i);
    list_appendn(&l, n);
  }

  s = list_size(&l);
  p = list_head(&l);
  for(int i = 0; i < s; i ++, p = p->next) {
    assert(p->data == i);
  }

  p = list_head(&l);
  for(int i = 0; p != list_tail(&l); i ++, p = p->next) {
    assert(p->data == i);
  }

  list_reverse(&l);
  s = list_size(&l);
  p = list_head(&l);
  for(int i = 0; i < s; i ++, p = p->next) {
    assert(p->data == s - i - 1);
  }

  list_reverse(&l);
  s = list_size(&l);
  p = list_head(&l);
  for(int i = 0; i < s; i ++, p = p->next) {
    assert(p->data == i);
  }

  int t = 1234;
  list_prepend(&l, t);
  assert(list_size(&l) == s + 1);
  assert(list_head(&l)->data = 1234);

  t = 2234;
  n = list_node_make(list_node_int_t, t);
  list_prependn(&l, n);
  assert(list_size(&l) == s + 2);
  assert(list_head(&l)->data = 2234);

  s = list_size(&l);
  p = list_head(&l);
  for(int i = 0; i < s; i ++, p = p->next) {
    if (i == 0) {
      assert(p->data == 2234);
    } else if (i == 1) {
      assert(p->data == 1234);      
    } else {
      assert(p->data == i - 2);
    }
  }

  t = 3344;
  list_insert(&l, 2, t);
  assert(list_size(&l) == s + 1);

  t = 4434;
  n = list_node_make(list_node_int_t, t);
  list_insertn(&l, 3, n);
  assert(list_size(&l) == s + 2);

  s = list_size(&l);
  p = list_head(&l);
  for(int i = 0; i < s; i ++, p = p->next) {
    if (i == 0) {
      assert(p->data == 2234);
    } else if (i == 1) {
      assert(p->data == 1234);      
    } else if (i == 2) {
      assert(p->data == 3344);      
    } else if (i == 3) {
      assert(p->data == 4434);      
    } else {
      assert(p->data == i - 4);
    }
  }

  i = 0;
  list_foreach_n(&l, p) {
    if (i == 0) {
      assert(p->data == 2234);
    } else if (i == 1) {
      assert(p->data == 1234);      
    } else if (i == 2) {
      assert(p->data == 3344);      
    } else if (i == 3) {
      assert(p->data == 4434);      
    } else {
      assert(p->data == i - 4);
    }
    i ++;
  }

  list_remove(&l, 0);
  assert(list_size(&l) == s - 1);
  assert(list_head(&l)->data == 1234);

  s = list_size(&l);
  p = list_head(&l);
  for(int i = 0; i < s; i ++, p = p->next) {
    if (i == 0) {
      assert(p->data == 1234);      
    } else if (i == 1) {
      assert(p->data == 3344);      
    } else if (i == 2) {
      assert(p->data == 4434);      
    } else {
      assert(p->data == i - 3);
    }
  }

  list_remove(&l, 2);
  assert(list_size(&l) == s - 1);

  s = list_size(&l);
  p = list_head(&l);
  for(int i = 0; i < s; i ++, p = p->next) {
    if (i == 0) {
      assert(p->data == 1234);      
    } else if (i == 1) {
      assert(p->data == 3344);      
    } else {
      assert(p->data == i - 2);
    }
  }

  list_remove(&l, s - 1);
  assert(list_size(&l) == s - 1);
  assert(list_tail(&l)->data == 8);

  s = list_size(&l);
  for(int i = 0; i < s; i ++) {
    list_getn(&l, i, &p);
    if (i == 0) {
      assert(p->data == 1234);
    } else if (i == 1) {
      assert(p->data == 3344);
    } else {
      assert(p->data == i - 2);
    }
  }

  t = 1111;
  list_set(&l, 1, t);
  assert(l.head->next->data == 1111);

  s = list_size(&l);
  for(int i = 0; i < s; i ++) {
    list_getn(&l, i, &p);
    if (i == 0) {
      assert(p->data == 1234);
    } else if (i == 1) {
      assert(p->data == 1111);
    } else {
      assert(p->data == i - 2);
    }
  }

  t = 1212;
  list_setn(&l, 2, list_node_make(list_int_t, t));
  assert(l.head->next->next->data == 1212);

  s = list_size(&l);
  for(int i = 0; i < s; i ++) {
    list_getn(&l, i, &p);
    if (i == 0) {
      assert(p->data == 1234);
    } else if (i == 1) {
      assert(p->data == 1111);
    } else if (i == 2) {
      assert(p->data = 1212);
    } else {
      assert(p->data == i - 2);
    }
  }

  list_swap(&l, 1, 2);
  s = list_size(&l);
  for(int i = 0; i < s; i ++) {
    list_getn(&l, i, &p);
    if (i == 0) {
      assert(p->data == 1234);
    } else if (i == 1) {
      assert(p->data == 1212);
    } else if (i == 2) {
      assert(p->data = 1111);
    } else {
      assert(p->data == i - 2);
    }
  }

  list_deinit(&l);

  return 0;
}
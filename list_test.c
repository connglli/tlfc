#include <stdio.h>
#include <stdlib.h>
#include "list.h"

list_template(int, list_int_t, list_node_int_t);

int main() {
	list_int_t l;
	list_init(&l);
	list_node_int_t *n, *p;
	void* v;
	int s;

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

	return 0;
}
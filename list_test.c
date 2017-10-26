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

	return 0;
}
#include "list.h"

/**
 * _list_node_make allocates and then initializes a node
 * @param  nodesz node size
 * @param  x      value to be added
 * @param  xsize  size of x
 * @return        allocated and initialized node
 */
void* _list_node_make(int nodesz, void* x, int xsize) {
	void* node = malloc(nodesz);
	if (node == NULL) { return node; }

	*_list_node_unsafe_nextp(node) = node;
	*_list_node_unsafe_prevp(node) = node;
	memcpy(_list_node_unsafe_datap(node), x, xsize);

	return node;
}

/**
 * _list_unsafe_appendn appends a node(usually of type void*) to l
 * @param  head   ptr to list head
 * @param  tail   ptr to list tail
 * @param  size   ptr to list size
 * @param  nodesz node size of this list
 * @param  node   node to be appended
 * @return        idx of node
 */
int _list_unsafe_appendn(char** head, char** tail, int* size, int nodesz,
												 void* node) {
	if (*size == 0) {
		*tail = *head = node;
	} else {
		*_list_node_unsafe_nextp(node) = *head;
		*_list_node_unsafe_prevp(node) = *tail;
		*_list_node_unsafe_nextp(*tail) = node;
		*_list_node_unsafe_prevp(*head) = node;
		*tail = node;
	}

	return (*size) ++;
}
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
 * @return        last index
 */
int _list_unsafe_appendn(char** head, char** tail, int* size, int nodesz,
                         void* node) {
  return _list_unsafe_insertn(head, tail, size, nodesz, *size, node);
}

/**
 * _list_unsafe_prependn prepends a node(usually of type void*) to l
 * @param  head   ptr to list head
 * @param  tail   ptr to list tail
 * @param  size   ptr to list size
 * @param  nodesz node size of this list
 * @param  node   node to be prepended
 * @return        0
 */
int _list_unsafe_prependn(char** head, char** tail, int* size, int nodesz,
                          void* node) {
  return _list_unsafe_insertn(head, tail, size, nodesz, -1, node);
}

/**
 * _list_unsafe_insertn inserts a node(usually of type void*) to l at idx,
 * if idx is valid, then insert it,
 * or if idx is negative, prepend it
 * or append it
 * @param  head   ptr to list head
 * @param  tail   ptr to list tail
 * @param  size   ptr to list size
 * @param  nodesz node size of this list
 * @param  idx    idx to be inserted
 * @param  node   node to be inserted
 * @return        idx of node
 */
int _list_unsafe_insertn(char** head, char** tail, int* size, int nodesz,
                         int idx, void* node) {
  if (*size == 0) {
    *_list_node_unsafe_nextp(node) = node;
    *_list_node_unsafe_prevp(node) = node;
    *tail = *head = node;
    *size = 1;
    return 0;
  }

  void *prev = NULL, *next = NULL;
  int inserted_idx;

  if (idx < 0) { // prepend it
    prev = *tail;
    next = *head;
    inserted_idx = 0;
  } else if (idx >= *size) { // append it
    prev = *tail;
    next = *head;
    inserted_idx = *size;
  } else {
    next = *head;
    for (int i = 0; i != idx; i ++) { // normally insert it
      next = *_list_node_unsafe_nextp(next);
    }
    prev = *_list_node_unsafe_prevp(next);
    inserted_idx = idx;
  }

  // prev <-> node <-> next
  *_list_node_unsafe_nextp(node) = next;
  *_list_node_unsafe_prevp(node) = prev;
  *_list_node_unsafe_nextp(prev) = node;
  *_list_node_unsafe_prevp(next) = node;
  if (idx < 0) { *head = node; } else if (idx >= *size) { *tail = node; }

  (*size) ++;

  return inserted_idx;
}

/**
 * _list_swap swaps value in idx1 and idx2
 * @param  head   ptr to list head
 * @param  tail   ptr to list tail
 * @param  size   ptr to list size
 * @param  nodesz node size of this list
 * @param  idx1   one index
 * @param  idx2   the other index
 * @return        0 if succeeded else -1
 */
int _list_swap(char** head, char** tail, int* size, int nodesz,
               int idx1, int idx2) {
  if (idx1 == idx2) { return 0; }

  int xsize = nodesz - 2 * sizeof(void*);
  int min_idx = idx1 < idx2 ? idx1 : idx2;
  int max_idx = idx1 < idx2 ? idx2 : idx1;
  void *min_ptr, *max_ptr, *n = *head;
  void *buf = (void*)malloc(xsize);
  if (buf == NULL) { return -1; }

  for (int i = 0; i < *size; i ++, n = *_list_node_unsafe_nextp(n)) {
    if (i == min_idx) { min_ptr = _list_node_unsafe_datap(n); }
    else if (i == max_idx) { max_ptr = _list_node_unsafe_datap(n); break; }
  }

  memcpy(buf, min_ptr, xsize);
  memcpy(min_ptr, max_ptr, xsize);
  memcpy(max_ptr, buf, xsize);

  free(buf);

  return 0;
}
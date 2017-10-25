#include "array.h"
/**
 * _array_force_expand reallocs the memory ignoring size and capacity
 * @param  data     ptr to the memory
 * @param  size     ptr to size of the contained items in array
 * @param  capacity ptr to capacity of the array
 * @param  itemsz   size of an array item
 * @return          0 for success, and -1 for failed
 */
int _array_force_expand(char** data, int* size, int* capacity, int itemsz) {
	void* ptr;
	int n;

	n = (*capacity == 0) ? 1 : (*capacity << 1);
	ptr = realloc(*data, n * itemsz);
	if (ptr == NULL) { return -1; }
	else { *data = ptr; *capacity = n; }

	return 0;
}

/**
 * _array_expand reallocs the memory if size exceeds the capacity
 * @param  data     ptr to the memory
 * @param  size     ptr to size of the contained items in array
 * @param  capacity ptr to capacity of the array
 * @param  itemsz   size of an array item
 * @return          0 for success, and -1 for failed
 */
int _array_expand(char** data, int* size, int* capacity, int itemsz) {
	if (*size < *capacity) { return 0; }
	return _array_force_expand(data, size, capacity, itemsz);
}

/**
 * _array_reverse reverses an array_t
 * @param  data     ptr to the memory
 * @param  size     ptr to size of the contained items in array
 * @param  capacity ptr to capacity of the array
 * @param  itemsz   size of an array item
 * @return          0 for success, and -1 for failed
 */
int _array_reverse(char** data, int* size, int* capacity, int itemsz) {
	if (*size == 0 || *size == 1) { return 0; }

	void* buf = malloc(itemsz);
	if (buf == NULL) { return -1; }
	void* head_ptr = *data;
	void* tail_ptr = *data + (*size - 1) * itemsz;
	int half = *size / 2;

	while(head_ptr < tail_ptr) {
		memmove(buf, head_ptr, itemsz);
		memmove(head_ptr, tail_ptr, itemsz);
		memmove(tail_ptr, buf, itemsz);
		head_ptr += itemsz;
		tail_ptr -= itemsz;
	}

	free(buf);

	return 0;
}

/**
 * _array_swap swap the array item idx1 with idx2
 * @param  data     ptr to the memory
 * @param  size     ptr to size of the contained items in array
 * @param  capacity ptr to capacity of the array
 * @param  itemsz   size of an array item
 * @param  idx1     index of one item
 * @param  idx2     index of the other item
 * @return          0 for success, and -1 for failed
 */
int _array_swap(char** data, int* size, int* capacity, int itemsz,
									 int idx1, int idx2) {
	if (idx1 == idx2) { return 0; }

	void* buf = malloc(itemsz);
	if (buf == NULL) { return -1; }
	void* ptr1 = *data + idx1 * itemsz;
	void* ptr2 = *data + idx2 * itemsz;

	memmove(buf, ptr1, itemsz);
	memmove(ptr1, ptr2, itemsz);
	memmove(ptr2, buf, itemsz);

	free(buf);

	return 0;
}

/**
 * _array_move moves the array item from from_idx to to_idx, 
 * if from_idx is same as to_idx, 
 *   nothing will happen,
 * if from_idx is less than to_idx, size will be automatically increased, 
 *   and all in them are set to 0
 * or decreased.
 * @param  data     ptr to the memory
 * @param  size     ptr to size of the contained items in array
 * @param  capacity ptr to capacity of the array
 * @param  itemsz   size of an array item
 * @param  from_idx index of start point
 * @param  to_idx   index of end point
 * @return          0 for success, and -1 for failed
 */
int _array_move(char** data, int* size, int* capacity, int itemsz, 
								int from_idx, int to_idx) {
	if (from_idx == to_idx) { return 1; }

	if (to_idx - from_idx + size >= capacity) {
		if(_array_force_expand(data, size, capacity, itemsz) == -1) {
			return -1;
		}
	}
	
	void* from_ptr = *data + from_idx * itemsz;
	void* to_ptr = *data + to_idx * itemsz;
	int memsz = (*size - from_idx) * itemsz;

	memmove(to_ptr, from_ptr, memsz);
	if (from_ptr < to_ptr) { memset(from_ptr, 0, (int)(to_ptr - from_ptr)); }
	*size += (to_idx - from_idx);

	return 0;
}
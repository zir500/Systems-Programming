#include "heap.h"

static void heap_up(heap_t *heap) {
	int element_index = heap->length; //Last element in the Heap
	while(element_index != 1){
		//Get Parent.
		int parent_index = (element_index/2);
		
		// Swap if 	parent is greater
		if (heap->store[parent_index-1] > heap->store[element_index-1]){
			swap_heap_elements(heap, parent_index-1, element_index-1);
		} else {
			break;
		}
		
		element_index = parent_index;
	}
}

static void heap_down(heap_t *heap) {
	int element_index = 1;
	int first_child_index = element_index *2;
	
	while (heap->length >= first_child_index+1){
		//2 Children
		if (heap->store[element_index-1] > heap->store[first_child_index-1] || heap->store[element_index-1] > heap->store[(first_child_index+1)-1]){
			// parent is bigger than either of its children then swap with the smallest
			if (heap->store[first_child_index-1] <= heap->store[first_child_index+1]){
				swap_heap_elements(heap, element_index-1, first_child_index-1);
				element_index = first_child_index;
			} else {
				swap_heap_elements(heap, element_index-1, first_child_index+1-1);
				element_index = first_child_index+1;
			}
		} else {
			break; // No smaller child
		}
		first_child_index = element_index *2;
	}
	
	// For odd number of elements in the heap.
	if (heap->length == first_child_index){
		if (heap->store[element_index-1] < heap->store[first_child_index-1]){
			swap_heap_elements(heap, element_index-1, first_child_index-1);
		}
	} 
}
	
void swap_heap_elements(heap_t *heap, int const first, int const second){
			int temp = heap->store[first];
			heap->store[first] = heap->store[second]; 
			heap->store[second] = temp; 
}

void heap_init(heap_t *heap, int *store) {
	heap->store = store;
	heap->length = 0;
}

void heap_insert(heap_t *heap, int value) {
	// The new element is always added to the end of a heap
	heap->store[(heap->length)++] = value;
	heap_up(heap);
}

int heap_extract(heap_t *heap) {
	// The root value is extracted, and the space filled by the value from the end
	// If the heap is empty, this will fail horribly...
	int value = heap->store[0];
	heap->store[0] = heap->store[--(heap->length)];
	heap_down(heap);
	return value;
}

int heap_isEmpty(heap_t *heap) {
	return !(heap->length);
}

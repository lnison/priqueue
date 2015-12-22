#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "pqueue.h"

#define MPANIC(x) ;if(x == NULL) { perror("Malloc failed."); exit(1); }
#define ITER_AND_NULL(arr,length) int _iter_i;for(_iter_i=0; _iter_i < length; _iter_i++) { arr[_iter_i] = NULL; }

static void __insert(Priqueue *heap, Node* node);
static Node *__pop(Priqueue *heap);

MHEAP_API Priqueue* priqueue_initialize(int initial_length){
  Priqueue *heap = (Priqueue *) malloc(sizeof(Priqueue)) MPANIC(heap);

  pthread_mutex_init(&(heap->lock), NULL);

  heap->head = NULL;
  heap->heap_size = initial_length;
  heap->occupied = 0;
  heap->current = 1;
  heap->array = malloc(initial_length * sizeof(*heap->array));

  ITER_AND_NULL(heap->array,initial_length);

  return heap;
}

static MHEAP_API MHEAPSTATUS realloc_heap(Priqueue *heap){

  if (heap->occupied >= heap->heap_size){
    void **tmp;
    tmp = realloc(heap->array,(2 * heap->heap_size) * sizeof(*heap->array));
    if (tmp != NULL){
      heap->heap_size *= 2;
      heap->array = (Node**) tmp;
      return MHEAP_OK;
    } else return MHEAP_REALLOCERROR;
  }

  return MHEAP_NOREALLOC;
}


MHEAP_API void priqueue_insert(Priqueue *heap, Data *data, int priority){

  Node *node = (Node *) malloc(sizeof(Node)) MPANIC(node);
  node->priority = priority;
  node->data = data;

  pthread_mutex_lock(&(heap->lock));
  __insert(heap,node);
  pthread_mutex_unlock(&(heap->lock));
}

static void __insert(Priqueue *heap, Node* node){

  if (heap->current == 1 && heap->array[1] == NULL){
    heap->head = node;
    heap->array[1] = node;
    heap->array[1]->index = heap->current;
    heap->occupied++;
    heap->current++;

    return;
  }

  if(heap->occupied >= heap->heap_size) realloc_heap(heap);
  if(heap->occupied <= heap->heap_size){
    node->index = heap->current;
    heap->array[heap->current] = node;

    int parent = (heap->current / 2);

    if (heap->array[parent]->priority < node->priority){
      heap->occupied++;
      heap->current++;
      int depth = heap->current/2;
      int traverse = node->index;
      while(depth >= 1){
	if (traverse == 1) break;
        if(heap->array[traverse/2]->priority < heap->array[traverse]->priority){
          int parent_index = heap->array[traverse/2]->index;
          int child_index = traverse;
          Node *tmp = heap->array[traverse/2];
          heap->array[traverse/2] = heap->array[traverse];
	  heap->array[traverse/2]->index = parent_index;
          heap->array[traverse] = tmp;
	  heap->array[traverse]->index = child_index;
          traverse = parent_index;
        }
	depth --;
      }
      heap->head = heap->array[1];
    } else {
      heap->occupied++;
      heap->current++;
    }
  }
}

MHEAP_API Node *priqueue_pop(Priqueue *heap){
  Node *node = NULL;
  
  pthread_mutex_lock(&(heap->lock));
  node = __pop(heap);
  pthread_mutex_unlock(&(heap->lock));

  return node;
}

static Node *__pop(Priqueue *heap){
  Node *node = NULL;
  int i;

  if (heap->current == 1) return node;
  if (heap->current == 2) {
    node = heap->array[1];
    heap->array[1] = NULL;
    heap->current -= 1;
    heap->occupied -= 1;

    return node;
  }

  if (heap->current >= 2 ){
    node = heap->array[1];
    heap->array[1] = heap->array[heap->current - 1];
    heap->current -= 1;
    heap->occupied -= 1;
    int depth = (heap->current - 1)/2;
    for(i = 1; i<=depth; i++){
      if (heap->array[i]->priority < heap->array[i*2]->priority || heap->array[i]->priority < heap->array[(i*2)+1]->priority){
	unsigned int biggest = heap->array[i*2]->priority > heap->array[(i*2)+1]->priority ? heap->array[(i*2)]->index : heap->array[(i*2)+1]->index;
	unsigned int currindex = heap->array[i]->index;
	Node *tmp = heap->array[i];
	heap->array[i] = heap->array[biggest];
	heap->array[i]->index = currindex;
	heap->array[biggest] = tmp;
	heap->array[biggest]->index = biggest;
      }
    }
  }

  return node;
}

MHEAP_API void priqueue_free(Priqueue *heap){
  if (heap->head != NULL) free(heap->head);
  free(*heap->array);
  free(heap->array);
  free(heap);
}

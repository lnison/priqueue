#ifndef MHEAP_H
#define MHEAP_H
#endif

#define MHEAP_API
typedef struct _data Data;
typedef struct _node Node;
typedef struct _heap Priqueue;

struct _data {
  unsigned int type;
  void *data;
};

struct _node {
  unsigned int priority;
  unsigned int index;
  struct _data *data;
};

struct _heap {
  struct _node *head;
  struct _node **array;
  unsigned int heap_size;
  unsigned int occupied;
  unsigned int current;
  pthread_mutex_t lock;
};

typedef enum {
  MHEAP_OK = 0,
  MHEAP_EMPTY,
  MHEAP_FAILED,
  MHEAP_REALLOCERROR,
  MHEAP_NOREALLOC,
  MHEAP_FATAL
}MHEAPSTATUS;


Priqueue *
priqueue_initialize(int);

void
priqueue_insert(Priqueue *, Data *, int);

Node *
priqueue_pop(Priqueue *);

void
priqueue_free(Priqueue *);

# priqueue

Thread Safe Priority Queue in C.

# Example
  
    volatile unsigned int cond = 0;
    
    void *consumer(void *arg){
      Node *d = (Node *) malloc(sizeof(Data));
      Priqueue *h = (Priqueue *)arg;
      usleep(10);
      for(;;){
        d = priqueue_pop(h);
        if (d != NULL){
          printf("\n %s %u\n",(char *)d->data->data,(unsigned int)pthread_self());
    			priqueue_node_free(h,d);
        }
        sched_yield();
        CHECK_COND(cond);
      }
    }
    
    int main(){
      pthread_t t;
      pthread_t t2;
    
      Priqueue *heap = priqueue_initialize(10);
    
      pthread_create(&t,NULL,consumer,(void *)heap);
      pthread_create(&t2,NULL,consumer,(void *)heap);
    
      Data *value = (Data *) malloc(sizeof(Data) * 100);
    
      unsigned int i;
      for(i = 0; i < 100; i++){
        value[i].type = 1;
        value[i].data = (char *) malloc(6* sizeof(char *));
        sprintf(value[i].data,"test %d.",i);
        priqueue_insert(heap,&value[i],i);
      }
    
      sleep(2);
    
      SWAP_COND(cond,0,1);
    
      pthread_join(t,NULL);
      pthread_join(t2,NULL);
    
      priqueue_free(heap);
    
      return 0;
    }

# API

Initialize Priority Queue with specified capacity . Returns NULL if failed to lock the mutex.

    Priqueue *priqueue_initialize(int);

Insert into Queue with specified priority.

    void priqueue_insert(Priqueue *, Data *, uintptr_t);

Pops the node with maximum priority ( head ) . 

    Node *priqueue_pop(Priqueue *);

Deallocate data node after poping.

    void priqueue_node_free(Priqueue *, Node *);

Deallocate memory. Also deallocates remaining nodes from queue .

    void priqueue_free(Priqueue *);
	
Create a new queue from all elements in current queue .

    Priqueue* priqueue_popall(Priqueue *queue);

# TODOs

1. [Lightweight Mutex](http://preshing.com/20120226/roll-your-own-lightweight-mutex/)
2. API to shrink array size.

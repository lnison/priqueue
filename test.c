#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "pqueue.h"

#define CHECK_COND(cond) if (__sync_bool_compare_and_swap(&cond,1,1)) break;
#define SWAP_COND(cond,a,b) while(1){if (__sync_bool_compare_and_swap(&cond,a,b)) break; }

volatile unsigned int cond = 0;

void *consumer(void *arg){
	Node *d = (Node *) malloc(sizeof(Data));
	Priqueue *h = (Priqueue *)arg;
	usleep(10);
	for(;;){
		d = priqueue_pop(h);
		if (d != NULL){
			printf("\n %s %u\n",(char *)d->data->data,(unsigned int)pthread_self());
			free(d->data->data);
			free(d);
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

	int i;
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

  free(value);
	priqueue_free(heap);

	return 0;
}

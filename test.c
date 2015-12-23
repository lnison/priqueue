/*
The MIT License (MIT)

Copyright (c) 2015 Mike Taghavi (mitghi) <mitghi@me.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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

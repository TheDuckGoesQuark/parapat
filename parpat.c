
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include "queue.h"

int gettask(tq tq) {

}


void puttask(tq tq,int v) {

}

void worker(tq tqinput, tq tqoutput) {

}

void makeThread(void (*Worker)(),long t) {

}

void createfarm(void (*Worker)(),int n) {

}

void createpipe(void (*Worker1)(),void (*Worker2)()) {

}

struct Alloc {
  int next;
  void *buf[MAX_TASKS*100];
} alloc;

typedef struct Alloc *Alloc;




Queue newQueue () {

}

tq newtq() {
  tq tq = newQueue();
  return(tq);
}


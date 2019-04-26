#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

Queue* queue;

typedef struct Foo {
    int value;
} Foo;

Foo* createFoo(int value) {
    Foo* foo = malloc(sizeof(Foo));
    foo->value = value;
    return foo;
}

void destroyFoo(Foo* foo) {
    free(foo);
}

void* addFoo(void* arg) {
    int selfId = *((int *) arg);
    printf("\nThread %d adding foo with value %d", selfId, selfId);
    Foo* foo = createFoo(selfId);
    enqueue(queue, foo);
    printf("\nThread %d finished adding foo with value %d", selfId, selfId);
    return NULL;
}

void* removeFoo(void* arg) {
    int selfId = *((int *) arg);
    printf("\nThread %d waiting for foo", selfId);
    Foo* foo = dequeue(queue);
    printf("\nThread %d removed foo with value %d", selfId, foo->value);
    destroyFoo(foo);
    return NULL;
}

void testWithNThreads(int nThreads) {
    pthread_t tid[nThreads];
    int i = 0;

    queue = createQueue(10);
    while (i < nThreads) {
        int* arg = malloc(sizeof(*arg));
        *arg = i;

        int err;
        if (i % 2 == 0) {
            err = pthread_create(&(tid[i]), NULL, addFoo, arg);
        } else {
            err = pthread_create(&(tid[i]), NULL, removeFoo, arg);
        }

        if (err != 0) {
            printf("\ncouldn't create thread : [%s]", strerror(err));
        }
        i++;
    }

    for (int j = 0; j < nThreads; j++) {
        pthread_join(tid[j], NULL);
    }

    printf("\nnThreads: %d", nThreads);

    destroyQueue(queue);
}

int main() {
    testWithNThreads(10);
}

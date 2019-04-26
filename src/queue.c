#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Element of queue with pointer to next element in line
typedef struct Node{
    struct Node* nextInLine;
    void* data;
} Node;

// Queue, with head, tail, current size, maximum size, and lock
struct Queue {
    Node* head;
    Node* tail;
    int currentSize;
    int maxSize;
    pthread_cond_t notifier;
    pthread_mutex_t mutex;
};

// Allocates memory on the heap for the queue,
// and returns a pointer to the queue
struct Queue* createQueue(size_t maxSize) {
    // Allocate memory for queue
    Queue* queue = malloc(sizeof(Queue));

    // Check malloc was successful
    if (!queue) return NULL;

    // Ensure queue is empty
    queue->head = NULL;
    queue->tail = NULL;
    queue->currentSize = 0;
    queue->maxSize = maxSize;

    // Initialize queue lock
    pthread_mutex_init(&(queue->mutex), NULL);
    // Initialize waiting lock
    pthread_cond_init(&(queue->notifier), NULL);

    return queue;
}

// Places data on the queue, or blocks until space is available on the queue
void enqueue(struct Queue* queue, void* data) {
    // Attempt to gain lock to queue
    pthread_mutex_lock(&(queue->mutex));

    if(queue->maxSize > 0) {
        // Block until able to insert into queue
        while (queue->currentSize >= queue->maxSize) {
            pthread_cond_wait(&(queue->notifier), &(queue->mutex));
        }
    }

    // Allocate memory for new node in queue
    Node *node = malloc(sizeof(Node));
    node->nextInLine= NULL;
    node->data = data;

    if (queue->currentSize== 0) {
        // Initialize queue
        queue->head = node;
        queue->tail = node;
    } else {
        // Add to tail of queue
        queue->tail->nextInLine = node;
        queue->tail = node;
    }

    queue->currentSize += 1;

    // Inform waiting threads of updated size
    pthread_cond_broadcast(&(queue->notifier));

    // Release lock
    pthread_mutex_unlock(&(queue->mutex));
}

// Removes element from queue. Blocks if the queue is empty
void* dequeue(Queue* queue) {
    // Gain queue lock
    pthread_mutex_lock(&(queue->mutex));

    // Wait until queue contains something
    while (queue->currentSize <= 0) {
        pthread_cond_wait(&(queue->notifier), &(queue->mutex));
    }

    // Get first element in queue
    Node* node = queue->head;

    // Remove element
    if (queue->currentSize == 1) {
        queue->head = NULL;
        queue->tail = NULL;
    } else {
        // Remove from head and put next in line to the front
        queue->head = node->nextInLine;
    }

    // Extract data from node
    void *data = node->data;
    // Free memory used by node
    free(node);

    queue->currentSize-= 1;
    // Inform waiting threads of size change
    pthread_cond_broadcast(&(queue->notifier));
    // Release lock on queue
    pthread_mutex_unlock(&(queue->mutex));

    return data;
}

// Free memory allocated to queue and all the elements in it
void destroyQueue(Queue* queue, void (*destroyElement)(void* element)) {
    // Check queue is valid
    if (!queue) return;

    // Get head of queue
    Node* next = queue->head;
    Node* curr;
    // Iterate over each element of queue and destroy its contents
    while (next) {
        curr = next;
        next = curr->nextInLine;
        (*destroyElement)(curr->data);
        free(curr);
    }

    // Free locks
    pthread_cond_destroy(&(queue->notifier));
    pthread_mutex_destroy(&(queue->mutex));

    // Free queue
    free(queue);
}

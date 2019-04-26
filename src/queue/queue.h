#include <stdlib.h>

typedef struct Queue Queue;

// Create blocking queue. Size determines at which point it blocks inserts.
Queue* createQueue(size_t max_size);

// Adds element to queue. Blocks if queue has reached maximum size.
void enqueue(Queue* q, void* element);

// Removes an element from the queue. Blocks if the queue is empty.
void* dequeue(Queue* q);

// Frees the memory allocated to the queue,
void destroyQueue(Queue* q);

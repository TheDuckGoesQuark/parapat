#include <stdbool.h>
#include "../queue/queue.h"

typedef struct Step Step;

// Allocate memory for step, requiring a
// Queue to take inputs from
// Function to apply to inputs
// Queue to place outputs
// Number of works to perform step
// Whether or not a null output from the function should be forwarded to the next step
// Whether or not this is the final step
Step* createStep(Queue* inputQueue, Queue* outputQueue, void* (*functionToApply)(), int numWorkerThreads, bool filterNulls, bool final);

// Join worker threads and free contents
// NOTE: ensure signalShutdownToWorkerThreads() is called first otherwise
// threads will never join.
void destroyStep(Step* step);

// Inserts as many NULL messages into the queue to ensure that each worker thread
// will receive one.
// NOTE: If other threads are accessing the input/output queue
// then ensure they also have NULLS to consume before calling join.
void signalShutdownToWorkerThreads(Step* step);

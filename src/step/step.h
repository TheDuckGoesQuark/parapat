#include "queue.h"

typedef struct Step Step;

// Allocate memory for step, requiring a
// Queue to take inputs from
// Function to apply to inputs
// Queue to place outputs
Step* createStep(Queue* inputQueue, Queue* outputQueue, void* (*functionToApply)());

// Free memory used for step. Does not destroy queues or their elements
void destroyStep(Step* step);

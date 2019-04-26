#include "step.h"

typedef struct Step {
    Queue* inputQueue;
    Queue* outputQueue;
    void* (*functionToApply)();
} Step;

// Allocate memory for step, requiring a
// Queue to take inputs from
// Function to apply to inputs
// Queue to place outputs
Step* createStep(Queue* inputQueue, Queue* outputQueue, void* (*functionToApply)()) {
    // Allocate memory for step
    Step* step = malloc(sizeof(Step));

    // Check malloc was successful
    if (!step) return NULL;

    step->inputQueue = inputQueue;
    step->outputQueue = outputQueue;
    step->functionToApply = functionToApply;

    return step;
}

// Free memory used for step. Does not destroy queues or their elements
void destroyStep(Step* step) {
    // Check step is valid
    if (!step) return;

    // Free step
    free(step);
}

// Removes an element from the input queue,
// applies the function at this step,
// adds it to the output queue.
void runStep(Step* step) {
    // Removes an element from the input queue,
    void* input = dequeue(step->inputQueue);
    // applies the function at this step,
    void* output = step->functionToApply(input);
    // adds it to the output queue.
    enqueue(step->outputQueue, output);
}

#include <pthread.h>
#include <stdbool.h>
#include "step.h"

typedef struct Step {
    Queue* inputQueue;
    Queue* outputQueue;
    void* (*functionToApply)();
    pthread_t workerThread[];
} Step;

// Removes an element from the input queue,
// applies the function at this step,
// adds it to the output queue.
void* runStep(Step* step) {
    void* output;

    do {
        // Removes an element from the input queue,
        void* input = dequeue(step->inputQueue);

        if(input) {
            // applies the function at this step,
            output = step->functionToApply(input);
            // adds it to the output queue.
            enqueue(step->outputQueue, output);
        } else {
            // trigger thread termination
            output = NULL;
        }

    } while (output);

    return NULL;
}

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
    pthread_create(&(step->workerThread), NULL, (void*) runStep, step);

    return step;
}

// Free memory used for step. Does not destroy queues or their elements
void destroyStep(Step* step) {
    // Check step is valid
    if (!step) return;

    // Free step
    free(step);
}

// Block until worker thread for step has terminated
void joinWorkerThread(Step* step) {
    void* result;
    // Insert null in order to terminate thread
    enqueue(step->inputQueue, NULL);
    // Wait for worker to receive null and terminate
    pthread_join(step->workerThread, &result);
}

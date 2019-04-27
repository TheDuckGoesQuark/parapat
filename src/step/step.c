#include <stdio.h>
#include <pthread.h>

#include "../task/task.h"
#include "step.h"

typedef struct Step {
    Queue* inputQueue;
    Queue* outputQueue;
    void* (*functionToApply)();
    int numWorkerThreads;
    pthread_t* workerThreads;
    bool filterNulls;
} Step;

// Removes an element from the input queue,
// applies the function at this step,
// adds it to the output queue.
void* runStep(Step* step) {
    Task* task;
    do {
        // Removes an element from the input queue,
        task = dequeue(step->inputQueue);

        // Terminate if task NULL
        if (task) {
            // applies the function at this step,
            void* input = getTaskData(task);
            void* output= step->functionToApply(input);

            // Only forward nulls if specified to do so
            if (output != NULL || (step->filterNulls != true)) {
                // adds it to the output queue.
                setTaskData(task, output);
                enqueue(step->outputQueue, task);

            } // discard
        }

    } while (task);

    return NULL;
}

// Allocate memory for step, requiring a
// Queue to take inputs from
// Function to apply to inputs
// Queue to place outputs
Step* createStep(Queue* inputQueue, Queue* outputQueue, void* (*functionToApply)(), int numWorkerThreads, bool filterNulls) {
    // Allocate memory for step
    Step* step = malloc(sizeof(Step));

    // Check malloc was successful
    if (!step) return NULL;

    step->inputQueue = inputQueue;
    step->outputQueue = outputQueue;
    step->functionToApply = functionToApply;
    step->numWorkerThreads = numWorkerThreads;
    step->filterNulls = filterNulls;

    pthread_t* workerThreads = malloc(sizeof(pthread_t) * numWorkerThreads);
    for(int i = 0; i < step->numWorkerThreads; ++i) {
        pthread_create(&(workerThreads[i]), NULL, (void*) runStep, step);
    }
    step->workerThreads = workerThreads;

    return step;
}

// Block until worker thread for step has terminated
void joinWorkerThreads(Step* step) {
    void* result;
    // Wait for worker to receive null and terminate
    for(int i = 0; i < step->numWorkerThreads; ++i) {
        pthread_join(step->workerThreads[i], &result);
    }
}

// wait for worker threads to termiante, then free memory used for step
void destroyStep(Step* step) {
    joinWorkerThreads(step);
    // Check step is valid
    if (!step) return;

    // Free step
    free(step->workerThreads);
    free(step);
}


// Inserts as many NULL messages into the queue to ensure that each worker thread
// will receive one.
// NOTE: If other threads are accessing the input/output queue
// then ensure they also have NULLS to consume before calling join.
void signalShutdownToWorkerThreads(Step* step) {
    // Insert enough nulls to terminate all worker threads
    for(int i = 0; i < step->numWorkerThreads; ++i) {
        enqueue(step->inputQueue, NULL);
    }
}

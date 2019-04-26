#include <stdatomic.h>
#include <stdbool.h>
#include "../queue/queue.h"
#include "../step/step.h"

int MAX_BUFFER_PER_STEP = 4;

typedef struct Pipeline {
    Queue** queues; // Pointer to list of queues
    Queue* input; // input queue to pipeline
    Queue* output; // output queue to pipeline
    int numSteps; // Number of steps in pipeline
    int dataInTransit; // Number of inputs currently being computed
    Step** steps; // pointer to first step in step array
} Pipeline;

// Calculate the number of queues necessary to support the number of steps
int calculateNumQueues(int numSteps) {
    return numSteps + 1;
}

// Create pipeline consisting of steps given in array.
// Steps will occur in order functions are given
Pipeline* createPipeline(void* (*functionSteps[])(), int numSteps) {
    // Allocate and construct queues
    int numQueues = calculateNumQueues(numSteps);
    Queue** queues = malloc(sizeof(Queue*) * numQueues);
    for(int i = 0; i < numQueues; ++i) {
        queues[i] = createQueue(MAX_BUFFER_PER_STEP);
    }

    // Allocate and construct steps
    Step** steps = malloc(sizeof(Step*) * numSteps);
    for(int i = 0; i < numSteps; ++i) {
        steps[i] = createStep(queues[i], queues[i+1], functionSteps[i], 1);
    }

    // Allocate and construct pipeline
    Pipeline* pipeline = malloc(sizeof(Pipeline));
    pipeline->queues = queues;
    pipeline->numSteps = numSteps;
    pipeline->steps = steps;
    pipeline->dataInTransit = 0;

    return pipeline;
}

// Frees all data allocated to the pipeline if pipeline has finished processing all inputs
// Returns 0 on success, and -1 otherwise
int destroyPipeline(Pipeline* pipeline) {
    if (pipeline->dataInTransit != 0) return -1;

    // Enqueue nulls to trigger step worker threads to termiante
    for(int i = 0; i < pipeline->numSteps; ++i) {
        signalShutdownToWorkerThreads(pipeline->steps[i]);
    }
    // Join worker threads and free steps in array
    for(int i = 0; i < pipeline->numSteps; ++i) {
        destroyStep(pipeline->steps[i]);
    }
    free(pipeline->steps);

    // Free queues
    int numQueues = calculateNumQueues(pipeline->numSteps);
    for(int i = 0; i < numQueues; ++i) {
        destroyQueue(pipeline->queues[i]);
    }
    free(pipeline->queues);

    // Free pipeline
    free(pipeline);
    return 0;
}

// Add all the pointers to the input data to the pipeline in a FIFO order
// (i.e. data[0] will be added to queue first)
// Blocks until all data is added to the queue.
void submitAllToPipeline(Pipeline* pipeline, void* data[], int numberOfInputs) {
    for(int i = 0; i < numberOfInputs; ++i) {
        enqueue(pipeline->input, data[i]);
    }
    pipeline->dataInTransit += numberOfInputs;
}

// Add the pointer to the input data to the pipeline
// Blocks until data item is added to the queue
void submitToPipeline(Pipeline* pipeline, void* data) {
    enqueue(pipeline->input, data);
    pipeline->dataInTransit++;
}

// Blocks until data is returned, or if no data is currently in pipeline
// return null immediately
void* removeFromPipeline(Pipeline* pipeline, int numberOfOutputsToDrain) {
    if (pipeline->dataInTransit == 0) return NULL;
    else {
        void* result = dequeue(pipeline->input);
        pipeline->dataInTransit--;
        return result;
    }
}

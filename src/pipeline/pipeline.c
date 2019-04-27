#include <stdatomic.h>
#include <stdio.h>
#include "../queue/queue.h"
#include "../step/step.h"
#include "../task/task.h"

typedef struct Pipeline {
    Queue** queues; // All queues in pipeline
    Queue* input; // input queue to pipeline and pointer to first queue in array of queues
    Queue* output; // output queue to pipeline and last queue in array of queues

    Step** steps; // pointer to first step in step array
    int numSteps; // Number of steps in pipeline

    Batch** batches; // pointer to batch pointer array
    int numBatches; // number of batches
} Pipeline;

// Calculate the number of queues necessary to support the number of steps
int calculateNumQueues(int numSteps) {
    return numSteps + 1;
}

// Create pipeline consisting of steps given in array.
// Steps will occur in order functions are given
// workerThreadPerStep assigns workerThreadAtStep[i] worker threads to functionSteps[i]
// filterSteps[i] determines if a NULL return value from functionSteps[i] means the value should be discarded
Pipeline* createPipeline(void* (*functionSteps[])(), int numSteps, int workerThreadsAtStep[], bool filterSteps[]) {

    // Allocate and construct queues
    int numQueues = calculateNumQueues(numSteps);
    Queue** queues = malloc(sizeof(Queue*) * numQueues);
    for(int i = 0; i < numQueues; ++i) {
        queues[i] = createQueue(workerThreadsAtStep[i]);
    }

    // Allocate and construct steps
    Step** steps = malloc(sizeof(Step*) * numSteps);
    for(int i = 0; i < numSteps; ++i) {
        steps[i] = createStep(queues[i], queues[i+1], functionSteps[i], workerThreadsAtStep[i], filterSteps[i]);
    }

    // Allocate and construct pipeline
    Pipeline* pipeline = malloc(sizeof(Pipeline));
    pipeline->queues = queues;
    pipeline->input = queues[0];
    pipeline->output = queues[numQueues - 1];
    pipeline->numSteps = numSteps;
    pipeline->steps = steps;
    pipeline->numBatches = 0;
    pipeline->batches = NULL;

    return pipeline;
}

// Checks if any batches in the pipeline are still being processed
bool stillProcessing(Pipeline* pipeline) {
    for (int i = 0; i < pipeline->numBatches; i++) {
        if (!batchCompleted(pipeline->batches[i])) return false;
    }
    return true;
}

// Frees all data allocated to the pipeline if pipeline has finished processing all inputs
// Returns 0 on success, and -1 otherwise
int destroyPipeline(Pipeline* pipeline) {
    if (stillProcessing(pipeline)) {
        printf("Can't destroy pipeline while still processing data.\n");
        return -1;
    }

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
void addBatch(Pipeline* pipeline, void* data[], int numberOfInputs) {
    Batch* batch = createBatch(data, numberOfInputs);
    for(int i = 0; i < numberOfInputs; ++i) {
        enqueue(pipeline->input, data[i]);
    }
    pipeline->taskCount+= numberOfInputs;
}

// Blocks until next batch is returned.
// Batches will be returned in the order they are submitted
// If no batches, returns null
void** getNextBatchOutput(Pipeline* pipeline) {

}

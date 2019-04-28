#include <stdbool.h>

typedef struct Pipeline Pipeline;

typedef struct Result {
    void** results; // address of pointer array, each pointing to an output
    int numResults; // number of outputs pointed to
} Result;

// Create pipeline consisting of steps given in array.
// Steps will occur in order functions are given
// workerThreadPerStep assigns workerThreadAtStep[i] worker threads to functionSteps[i]
// filterSteps[i] determines if a NULL return value from functionSteps[i] means the value should be discarded
Pipeline* createPipeline(void* (*functionSteps[])(), int numSteps, int workerThreadAtStep[], bool filterSteps[]);

// Frees the pipeline.
// Ensure pipeline has been drained (i.e. n inputs = n outputs) to avoid
// memory leaks.
// Return value
int destroyPipeline(Pipeline* pipeline);

// Add all the pointers to the input data to the pipeline in a FIFO order
// (i.e. data[0] will be added to queue first)
// Blocks until all data is added to the queue.
void addBatch(Pipeline* pipeline, void* data[], int numberOfInputs);

// Blocks until next batch is returned.
// Batches will be returned in the order they are submitted
// If no batches, returns null
Result* getNextBatchOutput(Pipeline* pipeline);

// Destroys the result struct returned by the pipeline and the pointer array to the outputs
void destroyResult(Result* result);


typedef struct Pipeline Pipeline;

// Create pipeline consisting of steps given in array.
// Steps will occur in order functions are given
Pipeline* createPipeline(void* (*functionSteps[])(), int numSteps);

// Frees the pipeline.
// Ensure pipeline has been drained (i.e. n inputs = n outputs) to avoid
// memory leaks.
void destroyPipeline(Pipeline* pipeline);

// Add all the pointers to the input data to the pipeline in a FIFO order
// (i.e. data[0] will be added to queue first)
// Blocks until all data is added to the queue.
void submitAllToPipeline(Pipeline* pipeline, void* data[], int numberOfInputs);

// Add the pointer to the input data to the pipeline
// Blocks until data item is added to the queue
void submitToPipeline(Pipeline* pipeline, void* data);

// Blocks until data is returned
void* removeFromPipeline(Pipeline* pipeline);

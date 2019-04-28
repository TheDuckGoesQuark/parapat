#include <stdbool.h>

typedef struct Task Task;

typedef struct Batch Batch;

// Retrieves the contents of the task
void* getTaskData(Task* task);

// Sets the contents of the task object
void setTaskData(Task* task, void* data);

// Creates a batch from the given input data
Batch* createBatch(void* data[], int nTasks);

// Destroys the given batch and all its tasks
void destroyBatch(Batch* batch);

// Returns true if all tasks in this batch are completed
bool batchCompleted(Batch* batch);

// Block until the given batch has been completed
void waitForBatchToComplete(Batch* batch);

// Retrieves the task at the given index
Task* getTask(Batch* batch, int taskIndex);

// Returns the number of tasks in the batch
int getBatchSize(Batch* batch);

// Increments the number of tasks that are completed for the batch this task belonged to
// Notes if task was filtered in pipeline or actually completed
void recordCompletedTask(Task* task, bool filtered);

// Creates a pointer array to each output from this batch
// Number of results need to be known if consumer wants
// to iterate
void** getResultPointerArray(Batch* batch);

// Gets the number of results from the batch
// Meaningless if called before completion
int getNumberOfResults(Batch* batch);

// FOR STEP TEST ****

// Creates a single task (for testing)
Task* createTask(void* data);

// Destroy task (for testing)
void destroyTask(Task* task);

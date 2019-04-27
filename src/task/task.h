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

// Retrieves the task at the given index
Task* getTask(Batch* batch, int taskIndex);

// Returns the number of tasks in the batch
int getBatchSize(Batch* batch);

// Increments the number of tasks that are completed for the batch this task belonged to
void recordCompletedTask(Task* task);


// FOR STEP TEST ****

// Creates a single task (for testing)
Task* createTask(void* data);

// Destroy task (for testing)
void destroyTask(Task* task);

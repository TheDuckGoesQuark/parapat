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

// Adds a task to the given batch
void addTask(Batch* batch, Task task);

// Returns true if all tasks in this batch are completed
bool batchCompleted(Batch* batch);

// Gets all the tasks from this batch
Task* getTasks(Batch* batch);

// Returns the number of tasks in the batch
int getBatchSize(Batch* batch);

#include <stdlib.h>
#include "task.h"

typedef struct Task {
    void* data; // Data to be operated on
    Batch* batch; // The batch this task belonged to
} Task;

typedef struct Batch {
    Task* tasks; // All tasks that are part of the batch, with the first task being the original pointer creating by malloc
    int nTasks; // The number of tasks that are part of this batch
    int nCompleted; // Number of tasks that are completed
} Batch;

// Retrieves the contents of the task
void* getTaskData(Task* task) {
    return task->data;
}

// Sets the contents of the task object
void setTaskData(Task* task, void* data) {
    task->data = data;
}

// Creates a batch instance for the given set of tasks
Batch* createBatch(void* data[], int nTasks) {
    // Create batch
    Batch* batch = malloc(sizeof(Batch));
    batch->nTasks = nTasks;
    batch->nCompleted = 0;

    // Allocate tasks and supply data
    Task* tasks = malloc(sizeof(Task) * nTasks);
    for (int i = 0; i < nTasks; ++i) {
        tasks[i].data = data[i];
        // provide link to batch
        tasks[i].batch = batch;
    }

    // Provide link to tasks
    batch->tasks = tasks;
    return batch;
}

// Destroys the given batch
void destroyBatch(Batch* batch) {
    if (!batch) return;

    if (batch->tasks) {
        free(batch->tasks);
    }

    free(batch);
}

// Returns true if all tasks in this batch are completed
bool batchCompleted(Batch* batch) {
    if (batch->nCompleted == batch->nTasks) return true;
    else return false;
}

// Gets the task at the given index
Task* getTask(Batch* batch, int taskIndex) {
    return &batch->tasks[taskIndex];
}

// Returns the number of tasks in this batch
int getBatchSize(Batch* batch) {
    return batch->nTasks;
}

// Increments the number of tasks that are completed for the batch this task belonged to
void recordCompletedTask(Task* task) {
    task->batch->nCompleted++;
}

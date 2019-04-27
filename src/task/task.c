#include <stdlib.h>
#include "task.h"

typedef struct Task {
    void* data;
    bool complete;
} Task;

typedef struct Batch {
    Task* tasks; // All tasks that are part of the batch, with the first task being the original pointer creating by malloc
    int nTasks; // The number of tasks that are part of this batch
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
    // Allocate tasks and supply data
    Task* tasks = malloc(sizeof(Task) * nTasks);
    for (int i = 0; i < nTasks; ++i) {
        tasks[i].data = data[i];
    }

    // Create batch
    Batch* batch = malloc(sizeof(Batch));
    batch->tasks = tasks;
    batch->nTasks = nTasks;
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

// Adds a task to the given batch
void addTask(Batch* batch, Task task) {
    batch->nTasks++;
    batch->tasks = realloc(batch->tasks, batch->nTasks);
    batch->tasks[batch->nTasks-1] = task;
}

// Returns true if all tasks in this batch are completed
bool batchCompleted(Batch* batch) {
    for (int i = 0; i < batch->nTasks; ++i) {
        if (!batch->tasks[i].complete) return false;
    }
    return true;
}

// Returns the number of tasks in this batch
int getBatchSize(Batch* batch) {
    return batch->nTasks;
}

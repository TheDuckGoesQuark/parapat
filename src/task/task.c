#include <stdlib.h>

typedef struct Task {
    void* data;
    int ordering;
    int batchNumber;
} Task;

// Creates a task to hold the given data
// and with the given order and batch value
Task* createTask(void* data, int ordering, int batchNumber) {
    Task* task = malloc(sizeof(Task));
    task->data = data;
    task-> ordering = ordering;
    task-> batchNumber = batchNumber;
    return task;
}

// Creates tasks for each data point,
// incrementing the order value from the startOrdering value for each point.
Task* createTasks(void* data[], int nTasks, int startOrdering, int batchNumber) {
    Task* tasks = malloc(sizeof(Task) * nTasks);
    for (int i = 0; i < nTasks; ++i) {
        tasks[i].data = data;
        tasks[i].ordering = startOrdering + i;
        tasks[i].batchNumber = batchNumber;
    }
    return tasks;
}

// Destroys task
void destroyTask(Task* task) {
    if(!task) return;
    else free(task);
}

// Retrieves the contents of the task
void* getTaskData(Task* task) {
    return task->data;
}

// Sets the contents of the task object
void setTaskData(Task* task, void* data) {
    task->data = data;
}

// Retrieves the ordering value
int getOrdering(Task* task) {
    return task->ordering;
}

// Sets the ordering value
void setOrdering(Task* task, int ordering) {
    task->ordering = ordering;
}

// Retrieves the batch number
int getBatchNumber(Task* task) {
    return task->batchNumber;
}

// Sets the ordering value
void setBatchNumber(Task* task, int batchNumber) {
    task->batchNumber = batchNumber;
}

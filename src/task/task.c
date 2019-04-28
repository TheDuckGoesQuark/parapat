#include <stdlib.h>
#include <pthread.h>
#include "task.h"

typedef struct Task {
    void* data; // Data to be operated on
    bool filtered; // If the data for this task was filtered i.e. should not be returned
    Batch* batch; // The batch this task belonged to
} Task;

typedef struct Batch {
    Task* tasks; // All tasks that are part of the batch, with the first task being the original pointer creating by malloc
    int nTasks; // The number of tasks that are part of this batch
    int nCompleted; // Number of tasks that are completed
    pthread_cond_t notifier; // signal when batch is complete
    pthread_mutex_t mutex; // lock for signalling
    int outputs; // number of outputs. calculated once completed
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
    batch->outputs = 0;

    // Allocate tasks and supply data
    Task* tasks = malloc(sizeof(Task) * nTasks);
    for (int i = 0; i < nTasks; ++i) {
        tasks[i].data = data[i];
        // provide link to batch
        tasks[i].batch = batch;
        tasks[i].filtered = false;
    }

    // Provide link to tasks
    batch->tasks = tasks;

    // Initialize lock
    pthread_mutex_init(&(batch->mutex), NULL);
    // Initialize waiting lock
    pthread_cond_init(&(batch->notifier), NULL);

    return batch;
}

// Destroys the given batch
void destroyBatch(Batch* batch) {
    if (!batch) return;

    if (batch->tasks) {
        free(batch->tasks);
    }

    pthread_cond_destroy(&(batch->notifier));
    pthread_mutex_destroy(&(batch->mutex));
    free(batch);
}

bool batchCompleted(Batch* batch) {
    return batch == NULL || batch->nCompleted == batch->nTasks;
}

// Returns true if all tasks in this batch are completed
void waitForBatchToComplete(Batch* batch) {
    // Gain batch lock
    pthread_mutex_lock(&(batch->mutex));

    // Wait until completed
    while (!batchCompleted(batch)) {
        pthread_cond_wait(&(batch->notifier), &(batch->mutex));
    }

    pthread_mutex_unlock(&(batch->mutex));
}

// Gets the task at the given index
Task* getTask(Batch* batch, int taskIndex) {
    return &batch->tasks[taskIndex];
}

// Returns the number of tasks in this batch
int getBatchSize(Batch* batch) {
    return batch->nTasks;
}

// Calculates the number of results for the output of the batch
void calculateNumberOfResults(Batch* batch)  {
    int count = 0;
    for (int i = 0; i < batch->nTasks; i++) {
        if (!(batch->tasks[i]).filtered) count++;
    }
    batch->outputs = count;
}

// Increments the number of tasks that are completed for the batch this task belonged to
void recordCompletedTask(Task* task, bool filtered) {
    task->filtered = filtered;

    if (!task->batch) return;

    Batch* batch = task->batch;
    // Gain batch lock
    pthread_mutex_lock(&(batch->mutex));

    batch->nCompleted++;
    if (batchCompleted(batch)) {
        // Notify any waiting threads that batch is now completed
        pthread_cond_broadcast(&(batch->notifier));
        // Assign to batch number of outputs
        calculateNumberOfResults(batch);
    }

    // Return lock
    pthread_mutex_unlock(&(batch->mutex));
}

// Creates a single task (for testing)
Task* createTask(void* data) {
    Task* task = malloc(sizeof(Task));
    task->data = data;
    task->batch = NULL;
    task->filtered= false;
    return task;
}

// Destroy task (for testing)
void destroyTask(Task* task) {
    if (!task) return;
    else free(task);
}

// Returns the number of outputs from the batch
int getNumberOfResults(Batch* batch) {
    return batch->outputs;
}

void** getResultPointerArray(Batch* batch)  {
    // Shouldn't be called when not complete
    if (!batchCompleted(batch)) return NULL;

    int numberOfOutputs = getNumberOfResults(batch);
    void** outputBuffer = malloc(sizeof(void*) * numberOfOutputs);

    // Provide pointer to outputs
    int accountedForOutputs = 0;
    int currentOutputIndex = 0;
    while (accountedForOutputs < numberOfOutputs) {
        // Get task
        Task* task = getTask(batch, currentOutputIndex);

        // If task completed normally (i.e. not filtered)
        if (task->filtered == false) {
            // Add output of task to result set
            outputBuffer[accountedForOutputs] = getTaskData(task);
            accountedForOutputs++;
        }

        currentOutputIndex++;
    }

    return outputBuffer;
}

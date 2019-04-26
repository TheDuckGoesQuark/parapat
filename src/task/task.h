typedef struct Task Task;

// Creates a task to hold the given data
// and with the given order and batch value
Task* createTask(void* data, int ordering, int batchNumber);

// Creates tasks for each data point,
// incrementing the order value from the startOrdering value for each point.
Task* createTasks(void* data[], int nTasks, int startOrdering, int batchNumber);

// Destroys task
void destroyTask(Task* task);

// Retrieves the contents of the task
void* getTaskData(Task* task);

// Sets the contents of the task object
void setTaskData(Task* task, void* data);

// Retrieves the ordering value
int getOrdering(Task* task);

// Sets the ordering value
void setOrdering(Task* task, int ordering);

// Retrieves the batch number
int getBatchNumber(Task* task);

// Sets the ordering value
void setBatchNumber(Task* task, int batchNumber);

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "../task/task.h"
#include "../queue/queue.h"
#include "step.h"

int* timesTwo(int* input) {
    *input = (*input) * 2;
    return input;
}

int* evenFilter(int* number) {
    if (*number % 2 == 0) return NULL;
    else return number;
}

int compareInt(const void* a, const void* b) {
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

// Run test using step with
// number of worker threads
// number of inputs
// Retrieve outputs before or after signalling for step to terminate
void testStep(int numWorkers, int numInputs, int retreiveAfter, char message[]) {
    printf("Start test %s\n", message);

    // Create test instance
    Queue* inputQueue = createQueue(numInputs);
    Queue* outputQueue = createQueue(numInputs);
    Step* step = createStep(inputQueue, outputQueue, (void*) timesTwo, numWorkers, false);

    // Create test cases
    Task* inputs[numInputs];
    int expectedOutputs[numInputs];
    for(int i = 0; i < numInputs; ++i) {
        // Alloc and add
        int* inputValue = malloc(sizeof(int));
        Task* task = createTask(inputValue, i, 1);
        inputs[i] = task;

        // Copy for expected
        int expected = i;
        expectedOutputs[i] = *timesTwo(&expected);
    }

    // Submit test cases
    for(int i = 0; i < numInputs; ++i) {
        enqueue(inputQueue, inputs[i]);
    }

    // Retrieve outputs before or after signalling to stop
    int outputs[numInputs];
    if (retreiveAfter == 1) {
        signalShutdownToWorkerThreads(step);
        for(int i = 0; i < numInputs; ++i) {
            Task* task = (Task*) (dequeue(outputQueue));
            outputs[i] = *((int*) getTaskData(task));
        }
    } else {
        for(int i = 0; i < numInputs; ++i) {
            Task* task = (Task*) (dequeue(outputQueue));
            outputs[i] = *((int*) getTaskData(task));
        }
        signalShutdownToWorkerThreads(step);
    }

    // Sort outputs for comparison (as they may arrive out of order)
    qsort(outputs, numInputs, sizeof(int), compareInt);

    // Validate results
    int pass = numInputs;
    for(int i = 0; i < numInputs; ++i) {
        int output = outputs[i];
        int expected = expectedOutputs[i];
        if (output != expected) {
            printf("FAIL: EXPECTED = %d ACTUAL = %d\n", expected, output);
        } else {
            pass--;
        }
    }

    // Print results
    if (pass == 0) {
        printf("TEST %s PASSED\n", message);
    } else {
        printf("TEST %s FAILED: %d/%d failed\n", message, numInputs, pass);
    }

    // Cleanup
    destroyStep(step);
    destroyQueue(inputQueue);
    destroyQueue(outputQueue);
    for(int i = 0; i < numInputs; ++i) {
        destroyTask(inputs[i]);
    }
}

void testFilterStep(int numInputs, int numWorkers, int retreiveAfter, char message[]) {
    printf("Start filter test %s\n", message);

    // Create test instance
    Queue* inputQueue = createQueue(numInputs);
    Queue* outputQueue = createQueue(numInputs);
    Step* step = createStep(inputQueue, outputQueue, (void*) evenFilter, numWorkers, true);

    // Count odd numbers in range
    int oddCount = 0;
    for(int i = 0; i < numInputs; i++) {
        if (i % 2 == 0) oddCount++;
    }

    // Create test cases
    Task* inputs[numInputs];
    int expectedOutputs[oddCount];
    for(int i = 0, j = 0; i < numInputs; ++i) {
        // Alloc and add
        int* inputValue = malloc(sizeof(int));
        Task* task = createTask(inputValue, i, 1);
        inputs[i] = task;

        // Copy for expected
        if (i % 2 != 0)  {
            expectedOutputs[j] = i;
            j++;
        }
    }

    // Submit test cases
    for(int i = 0; i < numInputs; ++i) {
        enqueue(inputQueue, inputs[i]);
    }

    // Retrieve outputs before or after signalling to stop
    int outputs[oddCount];
    if (retreiveAfter == 1) {
        signalShutdownToWorkerThreads(step);
        for(int i = 0; i < oddCount; ++i) {
            Task* task = (Task*) (dequeue(outputQueue));
            outputs[i] = *((int*) getTaskData(task));
        }
    } else {
        for(int i = 0; i < oddCount; ++i) {
            Task* task = (Task*) (dequeue(outputQueue));
            outputs[i] = *((int*) getTaskData(task));
        }
        signalShutdownToWorkerThreads(step);
    }

    // Sort outputs for comparison (as they may arrive out of order)
    qsort(outputs, numInputs, sizeof(int), compareInt);

    // Validate results
    int pass = false;
    for(int i = 0; i < oddCount; ++i) {
        int output = outputs[i];
        int expected = expectedOutputs[i];
        if (output != expected) {
            printf("FAIL: EXPECTED = %d ACTUAL = %d\n", expected, output);
        } else {
            pass--;
        }
    }

    // Print results
    if (pass == 0) {
        printf("TEST %s PASSED\n", message);
    } else {
        printf("TEST %s FAILED\n", message);
    }

    // Cleanup
    destroyStep(step);
    destroyQueue(inputQueue);
    destroyQueue(outputQueue);
    for(int i = 0; i < numInputs; ++i) {
        destroyTask(inputs[i]);
    }
}

int main() {
    for(int i = 1; i < 20; ++i) {
        for(int j = 0; j < 20; ++j) {
            char* falseTest = (char*) malloc(52 * sizeof(char));
            sprintf(falseTest, "[numworkers:%d | numinputs:%d | retriveAfter:false]", i, j);
            char* trueTest = (char*) malloc(52 * sizeof(char));
            sprintf(trueTest, "[numworkers:%d | numinputs:%d | retriveAfter:true ]", i, j);
            testStep(i, j, 0, falseTest);
            testStep(i, j, 1, trueTest);
        }
    }

    for(int i = 1; i < 20; ++i) {
        for(int j = 0; j < 20; ++j) {
            char* falseTest = (char*) malloc(80 * sizeof(char));
            sprintf(falseTest, "[numworkers:%d | numinputs:%d | retriveAfter:false | filter:true]", i, j);
            char* trueTest = (char*) malloc(80 * sizeof(char));
            sprintf(trueTest, "[numworkers:%d | numinputs:%d | retriveAfter:true | filter:true]", i, j);
            testFilterStep(i, j, 0, falseTest);
            testFilterStep(i, j, 1, trueTest);
        }
    }
}

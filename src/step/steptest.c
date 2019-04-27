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
    if (*number % 2 == 0) {
        return NULL;
    }
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
int testStep(int numWorkers, int numInputs, int retreiveAfter, char message[]) {

    // Create test instance
    Queue* inputQueue = createQueue(numInputs);
    Queue* outputQueue = createQueue(numInputs);
    Step* step = createStep(inputQueue, outputQueue, (void*) timesTwo, numWorkers, false, false);

    // Create test cases
    Task* inputs[numInputs];
    int inputValues[numInputs];
    int expectedOutputs[numInputs];
    for(int i = 0; i < numInputs; ++i) {
        // Alloc and add
        inputValues[i] = i;
        Task* task = createTask(&inputValues[i]);
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

    // Return 1 on success
    return pass == 0 ? 1 : 0;
}

int testFilterStep(int numWorkers, int numInputs, int retreiveAfter, char message[]) {
    // Create test instance
    Queue* inputQueue = createQueue(numInputs);
    Queue* outputQueue = createQueue(numInputs);
    Step* step = createStep(inputQueue, outputQueue, (void*) evenFilter, numWorkers, true, false);

    // Count odd numbers in range
    int oddCount = 0;
    for(int i = 0; i < numInputs; i++) {
        if (i % 2 != 0) oddCount++;
    }

    // Create test cases
    Task* inputs[numInputs];
    int inputValues[numInputs];
    int expectedOutputs[oddCount];
    for(int i = 0, j = 0; i < numInputs; ++i) {
        // Alloc and add
        inputValues[i] = i;
        Task* task = createTask(&inputValues[i]);
        inputs[i] = task;

        // Copy odd for expected
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
    qsort(outputs, oddCount, sizeof(int), compareInt);

    // Validate results
    int pass = true;
    for(int i = 0; i < oddCount; ++i) {
        int output = outputs[i];
        int expected = expectedOutputs[i];
        if (output != expected) {
            printf("FAIL: EXPECTED = %d ACTUAL = %d\n", expected, output);
            pass = false;
        }
    }

    // Print results
    if (pass == true) {
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

    return pass == true ? 1 : 0;
}

int main() {
    int totalCount = 0;
    int passCount = 0;
    char* falseTest = (char*) malloc(100 * sizeof(char));
    char* trueTest = (char*) malloc(100 * sizeof(char));
    for(int numWorkers = 1; numWorkers < 20; ++numWorkers) {
        for(int numInputs = 1; numInputs < 20; ++numInputs) {
            sprintf(falseTest, "[numworkers:%d | numinputs:%d | retriveAfter:false]", numWorkers, numInputs);
            sprintf(trueTest, "[numworkers:%d | numinputs:%d | retriveAfter:true ]", numWorkers, numInputs);

            passCount += testStep(numWorkers, numInputs, 0, falseTest);
            passCount += testStep(numWorkers, numInputs, 1, trueTest);
            totalCount += 2;
        }
    }

    printf("*** NORMAL TESTS: %d / %d PASSED ***\n", passCount, totalCount);
    totalCount = 0;
    passCount = 0;

    for(int numWorkers = 1; numWorkers < 20; ++numWorkers) {
        for(int numInputs = 0; numInputs < 20; ++numInputs) {
            sprintf(falseTest, "[numworkers:%d | numinputs:%d | retriveAfter:false | filter:true]", numWorkers, numInputs);
            sprintf(trueTest, "[numworkers:%d | numinputs:%d | retriveAfter:true | filter:true]", numWorkers, numInputs);
            passCount += testFilterStep(numWorkers, numInputs, 0, falseTest);
            passCount += testFilterStep(numWorkers, numInputs, 1, trueTest);
            totalCount += 2;
        }
    }
    printf("*** FILTER TESTS: %d / %d PASSED ***\n", passCount, totalCount);
    free(falseTest);
    free(trueTest);
    return 0;
}

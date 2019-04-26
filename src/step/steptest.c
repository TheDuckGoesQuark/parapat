#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "../queue/queue.h"
#include "step.h"

int* timesTwo(int* input) {
    *input = (*input) * 2;
    return input;
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
    Step* step = createStep(inputQueue, outputQueue, (void*) timesTwo, numWorkers);

    // Create test cases
    int* inputs[numInputs];
    int expectedOutputs[numInputs];
    for(int i = 0; i < numInputs; ++i) {
        // Alloc and add
        inputs[i] = malloc(sizeof(int));
        *inputs[i] = i;

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
            outputs[i] = *((int*) (dequeue(outputQueue)));
        }
    } else {
        for(int i = 0; i < numInputs; ++i) {
            outputs[i] = *((int*) (dequeue(outputQueue)));
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
            printf("FAIL: INPUT = %d EXPECTED = %d ACTUAL = %d\n", *inputs[i], expected, output);
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
        free(inputs[i]);
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
}

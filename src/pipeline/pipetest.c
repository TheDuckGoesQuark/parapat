#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "pipeline.h"

// Some structs to test with
typedef struct Foo {
    int value;
} Foo;

typedef struct Bar {
    int value;
} Bar;

Foo* createFoo(int value) {
    Foo* foo = malloc(sizeof(Foo));
    foo->value = value;
    return foo;
}

void destroyFoo(Foo* foo) {
    free(foo);
}

Bar* createBar(int value) {
    Bar* bar = malloc(sizeof(Bar));
    bar->value = value;
    return bar;
}

void destroyBar(Bar* bar) {
    free(bar);
}

Foo* timesFooByTwo(Foo* foo) {
    foo->value *= 2;
    return foo;
}

Foo* filterEvenFoos(Foo* foo) {
    if (foo->value % 2 == 0) return NULL;
    else return foo;
}

Bar* mapFooToBar(Foo* foo) {
    Bar* bar = malloc(sizeof(Bar));
    bar->value = foo->value;
    return bar;
}

// Run test using step with
// number of worker threads
// number of inputs
// Retrieve outputs before or after signalling for step to terminate
int testStep(int numWorkers[], int numInputsInBatch, int numBatches, char message[]) {

    int numSteps = 3
    void* (*functionSteps[numSteps])();
    functionSteps[0] = (void*) filterEvenFoos;
    functionSteps[1] = (void*) timesFooByTwo,
    functionSteps[2] = (void*) mapFooToBar;
    bool filterSteps[3] = {true, false, false};

    // Create test instance
    Pipeline* pipeline = createPipeline(functionSteps, numSteps, numWorkers, filterSteps);

    // Create test cases
    Task* inputs[numInputs];
    int inputValues[numInputs];
    int expectedOutputs[numInputs];
    for(int i = 0; i < numInputs; ++i) {
        // Alloc and add
        inputValues[i] = i;
        Task* task = createTask(&inputValues[i], i, 1);
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
    for(int i = 0; i < numInputs; ++i) {
    }

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
    destroyPipeline(pipeline);

    // Return 1 on success
    return pass == 0 ? 1 : 0;
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
    free(falseTest);
    free(trueTest);
    return 0;
}

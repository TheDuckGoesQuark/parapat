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
    if (foo->value % 2 == 0) {
        return NULL;
    }
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
int testStep(int numWorkers[], int numInputsInBatch, int numBatches, char message[]) {
    int numSteps = 3;
    void* (*functionSteps[numSteps])();
    functionSteps[0] = (void*) filterEvenFoos;
    functionSteps[1] = (void*) timesFooByTwo,
    functionSteps[2] = (void*) mapFooToBar;
    bool filterSteps[3] = {true, false, false};

    // Create test instance
    Pipeline* pipeline = createPipeline(functionSteps, numSteps, numWorkers, filterSteps);

    // Create test case buffers
    Foo* inputs[numBatches][numInputsInBatch];

    // Create test cases
    Bar* expectedOutputs[numBatches][numInputsInBatch];
    for(int i = 0; i < numBatches; ++i) {
        int outputIndex = 0;
        for (int j = 0; j < numInputsInBatch; j++) {
            int inputValue = ((i + 1) * (j + 1));
            inputs[i][j] = createFoo(inputValue);

            // even inputs are filtered at first step, so only include outputs
            if (inputValue % 2 != 0) {
                // Odd inputs are multiplied by two and mapped to a Bar
                expectedOutputs[i][outputIndex] = createBar(inputValue * 2);
                outputIndex++;
            }
        }
    }

    // Submit test cases
    for(int i = 0; i < numBatches; ++i) {
        addBatch(pipeline, (void*) inputs[i], numInputsInBatch);
    }

    // Retrieve outputs
    Bar* outputs[numBatches];
    int outputsPerBatch[numBatches];
    for(int i = 0; i < numBatches; ++i) {
        Result* batchOutput = getNextBatchOutput(pipeline);
        // Get address of outputs from result set
        void** results = batchOutput->results;
        Bar* bar = results[i];

        outputs[i] = *((Bar**) (batchOutput->results));
        outputsPerBatch[i] = batchOutput->numResults;
        // Destroy result struct
        destroyResult(batchOutput);
    }

    // Validate results
    int failed = 0;
    for(int i = 0; i < numBatches; ++i) {
        for (int j = 0; j < outputsPerBatch[i]; j++) {
            int output = outputs[i][j].value;
            int expected = expectedOutputs[i][j]->value;

            if (output != expected) {
                printf("FAIL: EXPECTED = %d ACTUAL = %d\n", expected, output);
                failed++;
            }
        }
    }

    // Print results
    if (failed == 0) {
        printf("TEST %s PASSED\n", message);
    } else {
        printf("TEST %s FAILED: %d/%d failed\n", message, failed, numInputsInBatch*numBatches);
    }

    // Cleanup
    printf("hello\n");
    destroyPipeline(pipeline);
    for(int i = 0; i < numBatches; ++i) {
        for (int j = 0; j < numInputsInBatch; j++) {
            destroyFoo(inputs[i][j]);

            if (expectedOutputs[i][j])
                destroyBar(expectedOutputs[i][j]);
        }
    }

    // Return 1 on success
    return failed == 0 ? 1 : 0;
}

int main() {
    int totalCount = 0;
    int passCount = 0;
    char* test = (char*) malloc(100 * sizeof(char));

    for(int numWorkers = 1; numWorkers < 3; ++numWorkers) {
        int numWorkersAtStep[3] = {numWorkers, numWorkers, numWorkers};

        for(int numBatches = 1; numBatches < 2; ++numBatches) {
            for(int numInputsInBatch = 1; numInputsInBatch < 2; ++numInputsInBatch) {
                sprintf(test, "[numworkers:%d | numbatches:%d | numinputs:%d]", numWorkers, numBatches, numInputsInBatch);
                passCount += testStep(numWorkersAtStep, numInputsInBatch, numBatches, test);
                totalCount += 1;
            }
        }
    }

    printf("*** NORMAL TESTS: %d / %d PASSED ***\n", passCount, totalCount);
    free(test);
    return 0;
}

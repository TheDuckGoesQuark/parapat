#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "parapat.h"


// Some structs to test with
typedef struct Foo {
    int value;
    int index;
    int batch;
    struct timespec* sleep;
} Foo;

typedef struct Bar {
    int value;
} Bar;

Foo ALL_FOOS[3][1000];
Bar ALL_BARS[3][1000];

Foo* createFoo(int value, int batch, int index, struct timespec* sleep) {
    ALL_FOOS[batch][index].value = value;
    ALL_FOOS[batch][index].sleep = sleep;
    ALL_FOOS[batch][index].index = index;
    ALL_FOOS[batch][index].batch = batch;
    return &(ALL_FOOS[batch][index]);
}

Bar* createBar(int value, int batch, int index) {
    ALL_BARS[batch][index].value = value;
    return &(ALL_BARS[batch][index]);
}

Foo* filterEvenFoos(Foo* foo) {
    nanosleep(&(foo->sleep[0]), NULL);
    if (foo->value % 2 == 0) {
        return NULL;
    }
    else return foo;
}

Foo* timesFooByTwo(Foo* foo) {
    nanosleep(&(foo->sleep[1]), NULL);
    foo->value *= 2;
    sleep(1);
    return foo;
}

Bar* mapFooToBar(Foo* foo) {
    nanosleep(&(foo->sleep[2]), NULL);
    return createBar(foo->value, foo->batch, foo->index);
}

// Perform same operations as timePipe but without parallelisation
void timeLinear(struct timespec* sleepTimes, int numBatches, int numInputsInBatch) {
    fprintf(stderr, "linear,%d,%d,%d,%lld.%.9ld,%lld.%.9ld,%lld.%.9ld,%d,%d\n",
        0, 0, 0,
        (long long)sleepTimes[0].tv_sec, sleepTimes[0].tv_nsec,
        (long long)sleepTimes[1].tv_sec, sleepTimes[1].tv_nsec,
        (long long)sleepTimes[2].tv_sec, sleepTimes[2].tv_nsec,
        numBatches,
        numInputsInBatch
    );
    // Create test case buffers
    Foo* inputs[numBatches][numInputsInBatch];

    // Create test cases
    for(int i = 0; i < numBatches; ++i) {
        for (int j = 0; j < numInputsInBatch; j++) {
            int inputValue = ((i + 1) * (j + 1));
            inputs[i][j] = createFoo(inputValue, i, j, sleepTimes);
        }
    }

    struct timespec start, finish;
    double elapsed;
    // BEGIN TIMING
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Run
    Bar* outputs[numBatches][numInputsInBatch];
    for(int i = 0; i < numBatches; ++i) {
        for (int j = 0; j < numInputsInBatch; ++j) {
            Foo* maybeFoo = filterEvenFoos(inputs[i][j]);
            if (!maybeFoo) {
                outputs[i][j] = NULL;
                continue;
            }
            Foo* fooTimesTwo = timesFooByTwo(maybeFoo);
            outputs[i][j] = mapFooToBar(fooTimesTwo);
        }
    }
    // END TIMING ONCE PROCESSING FINISHED

    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    // Print results
    printf("linear,%d,%d,%d,%lld.%.9ld,%lld.%.9ld,%lld.%.9ld,%d,%d,%lld\n",
        0, 0, 0,
        (long long)sleepTimes[0].tv_sec, sleepTimes[0].tv_nsec,
        (long long)sleepTimes[1].tv_sec, sleepTimes[1].tv_nsec,
        (long long)sleepTimes[2].tv_sec, sleepTimes[2].tv_nsec,
        numBatches,
        numInputsInBatch,
        (long long) elapsed
    );
}

int numSteps = 3;
void* (*functionSteps[3])();

    bool filterSteps[3] = {true, false, false};
// numWorkers[i]    = number of threads at step i
// sleepTimes[i]    = how long should step i take (nanosecs)
// numBatches       = number of batches to insert
// numInputsInBatch = number of inputs in each batch
void timePipe(int numWorkers[], struct timespec* sleepTimes, int numBatches, int numInputsInBatch) {
    fprintf(stderr, "pipe,%d,%d,%d,%lld.%.9ld,%lld.%.9ld,%lld.%.9ld,%d,%d\n",
        numWorkers[0], numWorkers[1], numWorkers[2],
        (long long)sleepTimes[0].tv_sec, sleepTimes[0].tv_nsec,
        (long long)sleepTimes[1].tv_sec, sleepTimes[1].tv_nsec,
        (long long)sleepTimes[2].tv_sec, sleepTimes[2].tv_nsec,
        numBatches,
        numInputsInBatch
    );


    // Create test case buffers
    Foo* inputs[numBatches][numInputsInBatch];

    // Create mock data
    for(int i = 0; i < numBatches; ++i) {
        for (int j = 0; j < numInputsInBatch; j++) {
            int inputValue = ((i + 1) * (j + 1));
            inputs[i][j] = createFoo(inputValue, i, j, sleepTimes);
        }
    }

    struct timespec start, finish;
    double elapsed;
    // BEGIN TIMING FROM LIB INIT
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create test instance
    Pipeline* pipeline = createPipeline(functionSteps, numSteps, numWorkers, filterSteps);

    // Submit test cases
    for(int i = 0; i < numBatches; ++i) {
        addBatch(pipeline, (void*) inputs[i], numInputsInBatch);
    }

    // Retrieve outputs
    Bar* outputs[numBatches][numInputsInBatch];
    int outputsPerBatch[numBatches];
    for(int i = 0; i < numBatches; ++i) {
        Result* batchOutput = getNextBatchOutput(pipeline);

        // Get address of outputs from result set
        Bar** resultPointerArray = (Bar**) batchOutput->results;
        outputsPerBatch[i] = batchOutput->numResults;

        for(int j = 0; j < outputsPerBatch[i]; j++) {
            outputs[i][j] = (resultPointerArray[j]);
        }

        // Destroy result struct
        destroyResult(batchOutput);
    }

    // END TIMING ONCE PROCESSING FINISHED
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    // Print results
    printf("pipe,%d,%d,%d,%lld.%.9ld,%lld.%.9ld,%lld.%.9ld,%d,%d,%lld\n",
        numWorkers[0], numWorkers[1], numWorkers[2],
        (long long)sleepTimes[0].tv_sec, sleepTimes[0].tv_nsec,
        (long long)sleepTimes[1].tv_sec, sleepTimes[1].tv_nsec,
        (long long)sleepTimes[2].tv_sec, sleepTimes[2].tv_nsec,
        numBatches,
        numInputsInBatch,
        (long long) elapsed
    );
}

void testWithWorkers(int numWorkers[3]) {
    struct timespec sleepTimes[3];
    sleepTimes[0].tv_sec = 0;
    sleepTimes[0].tv_nsec = 2;
    sleepTimes[1].tv_sec = 0;
    sleepTimes[1].tv_nsec = 1;
    sleepTimes[2].tv_sec = 0;
    sleepTimes[2].tv_nsec = 3;

    int sleepScaler = 10;
    while (sleepTimes[1].tv_nsec < 1000) {
        // Multiple sleep time by 10
        sleepTimes[0].tv_nsec *= sleepScaler;
        sleepTimes[1].tv_nsec *= sleepScaler;
        sleepTimes[2].tv_nsec *= sleepScaler;
        for(int numItemsPerBatch = 1; numItemsPerBatch <= 100; numItemsPerBatch*=10) {
            timePipe(numWorkers, sleepTimes, 1, numItemsPerBatch);
        }
    }
}

void testLinear() {
    struct timespec sleepTimes[3];
    sleepTimes[0].tv_sec = 0;
    sleepTimes[0].tv_nsec = 2;
    sleepTimes[1].tv_sec = 0;
    sleepTimes[1].tv_nsec = 1;
    sleepTimes[2].tv_sec = 0;
    sleepTimes[2].tv_nsec = 3;

    int sleepScaler = 10;
    while (sleepTimes[1].tv_nsec < 1000) {
        // Multiple sleep time by 10
        sleepTimes[0].tv_nsec *= sleepScaler;
        sleepTimes[1].tv_nsec *= sleepScaler;
        sleepTimes[2].tv_nsec *= sleepScaler;
        // For each number of batches
        for(int numItemsPerBatch = 1; numItemsPerBatch <= 100; numItemsPerBatch*=10) {
            timeLinear(sleepTimes, 1, numItemsPerBatch);
        }
    }
}

int main() {
    fprintf(stderr, "linear/pipe, numWorkers[0], numWorkers[1], numWorkers[2], sleepTimes[0], sleepTimes[1], sleepTimes[2], numBatches, numInputsInBatch\n");
    printf("linear/pipe, numWorkers[0], numWorkers[1], numWorkers[2], sleepTimes[0], sleepTimes[1], sleepTimes[2], numBatches, numInputsInBatch, runtime\n");
    int p = 1;
    int f = 3;
    int numWorkers[3];
    functionSteps[0] = (void*) filterEvenFoos;
    functionSteps[1] = (void*) timesFooByTwo;
    functionSteps[2] = (void*) mapFooToBar;

    // First step P
    numWorkers[0] = p;
    // Second step P
    numWorkers[1] = p;
    for(int i = 0; i < 2; i++) {
        // third step p
        numWorkers[2] = p;
        testWithWorkers(numWorkers);
        // third step f
        numWorkers[2] = f;
        testWithWorkers(numWorkers);
    }
    numWorkers[1] = f;
    for(int i = 0; i < 2; i++) {
        // third step p
        numWorkers[2] = p;
        testWithWorkers(numWorkers);
        // third step f
        numWorkers[2] = f;
        testWithWorkers(numWorkers);
    }

    // First step F
    numWorkers[0] = f;
    // Second step P
    numWorkers[1] = p;
    for(int i = 0; i < 2; i++) {
        // third step p
        numWorkers[2] = p;
        testWithWorkers(numWorkers);
        // third step f
        numWorkers[2] = f;
        testWithWorkers(numWorkers);
    }
    numWorkers[1] = f;
    for(int i = 0; i < 2; i++) {
        // third step p
        numWorkers[2] = p;
        testWithWorkers(numWorkers);
        // third step f
        numWorkers[2] = f;
        testWithWorkers(numWorkers);
    }

    testLinear();


    return 0;
}

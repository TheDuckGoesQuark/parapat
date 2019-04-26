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

void testStep() {
    Queue* inputQueue = createQueue(1);
    Queue* outputQueue = createQueue(1);
    Step* step = createStep(inputQueue, outputQueue, (void*) timesTwo);

    int original = 2;
    int value = original;
    enqueue(inputQueue, &value);
    runStep(step);
    int* result = dequeue(outputQueue);
    printf("Expected = %d\n", *timesTwo(&original));
    printf("Output = %d\n", *result);
}

int main() {
    testStep();
}

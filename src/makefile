TOPDIR="./"
LDFLAGS = -lGL -lm -lpthread -O3

queuetest: queue/queuetest.c queue/queue.h queue/queue.c
	$(CC) -o $@ $^ $(LDFLAGS)

steptest: step/steptest.c step/step.h step/step.c queue/queue.h queue/queue.c task/task.h task/task.c
	$(CC) -o $@ $^ $(LDFLAGS)

pipetest: step/steptest.c step/step.h step/step.c queue/queue.h queue/queue.c task/task.h task/task.c pipeline/pipeline.c pipeline/pipeline.h
	$(CC) -o $@ $^ $(LDFLAGS)

all: queuetest steptest pipetest

.PHONY: clean
clean:
	rm -f $(obj) *.o queuetest steptest pipetest

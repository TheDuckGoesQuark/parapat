TOPDIR="./"
LDFLAGS = -lGL -lm -lpthread -O3

queuetest: queue/queuetest.c queue/queue.h queue/queue.c
	$(CC) -o $@ $^ $(LDFLAGS)

steptest: step/steptest.c step/step.h step/step.c queue/queue.h queue/queue.c task/task.h task/task.c
	$(CC) -o $@ $^ $(LDFLAGS)

pipetest: pipeline/pipetest.c pipeline/pipeline.c pipeline/pipeline.h step/step.h step/step.c queue/queue.h queue/queue.c task/task.h task/task.c
	$(CC) -o $@ $^ $(LDFLAGS)

pipeline.o: pipeline/pipeline.c
	$(CC) -o $@ -c $^ $(LDFLAGS)

step.o: step/step.c
	$(CC) -o $@ -c $^ $(LDFLAGS)

task.o: task/task.c
	$(CC) -o $@ -c $^ $(LDFLAGS)

queue.o: queue/queue.c
	$(CC) -o queue.o -c queue/queue.c $(LDFLAGS)

parapat: pipeline.o step.o task.o queue.o
	ar rcs $@ $^

all: queuetest steptest pipetest parapat cleanobjs

.PHONY: clean
clean:
	rm -f $(obj) *.o queuetest steptest pipetest parapat

cleanobjs:
	rm -f $(obj) *.o

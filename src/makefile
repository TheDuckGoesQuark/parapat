TOPDIR="./"
LDFLAGS = -lGL -lm -lpthread -O3 -I $TOPDIR/queue -I $TOPDIR/step -I %TOPDIR/task

queuetest: queue/queuetest.c queue/queue.h queue/queue.c
	$(CC) -o $@ $^ $(LDFLAGS)

steptest: step/steptest.c step/step.h step/step.c queue/queue.h queue/queue.c task/task.h task/task.c
	$(CC) -o $@ $^ $(LDFLAGS)

all: queuetest steptest

.PHONY: clean
clean:
	rm -f $(obj) *.o queuetest steptest

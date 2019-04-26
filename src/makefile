LDFLAGS = -lGL -lm -lpthread -O3

queuetest: queue/queuetest.c queue/queue.h queue/queue.c
	$(CC) -o $@ $^ $(LDFLAGS)

all: queuetest

.PHONY: clean
clean:
	rm -f $(obj) *.o queuetest

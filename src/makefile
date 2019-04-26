LDFLAGS = -lGL -lm -lpthread -O3

queuetest: queuetest.c queue.h queue.c
	$(CC) -o $@ $^ $(LDFLAGS)

all: queuetest

.PHONY: clean
clean:
	rm -f $(obj) *.o queuetest

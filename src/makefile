LDFLAGS = -lGL -lm -lpthread -O3

test: main.c queue.h queue.c 
	$(CC) -o $@ $^ $(LDFLAGS)

all: test

.PHONY: clean
clean:
	rm -f $(obj) *.o test

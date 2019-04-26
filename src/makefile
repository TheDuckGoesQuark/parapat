LDFLAGS = -lGL -lm -lpthread -O3

farm.a: class.o
    ar -rv libclassdll.a class.o

pipeline.a: parpat.c queue.h
	$(CC) -o $@ $^ $(LDFLAGS)

all: farm

.PHONY: clean
clean:
	rm -f $(obj) *.o farm

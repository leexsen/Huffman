CFLAGS=-Wall -Werror -Os -c 

all: buffer.o encoder.o decoder.o hf.o queue.o 
	cc -o hf buffer.o encoder.o decoder.o hf.o queue.o

buffer.o: buffer.c
	cc ${CFLAGS} -o buffer.o buffer.c

encoder.o: encoder.c
	cc ${CFLAGS} -o encoder.o encoder.c

decoder.o: decoder.c
	cc ${CFLAGS} -o decoder.o decoder.c

hf.o: hf.c
	cc ${CFLAGS} -o hf.o hf.c

queue.o: queue.c
	cc ${CFLAGS} -o queue.o queue.c

clean:
	rm *.o hf

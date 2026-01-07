CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50

OBJS = pagedir.o index.o word.o

common.a: $(OBJS)
	ar cr common.a $(OBJS)

pagedir.o: pagedir.c pagedir.h
	$(CC) $(CFLAGS) -c pagedir.c

index.o: index.c index.h
	$(CC) $(CFLAGS) -c index.c

word.o: word.c word.h
	$(CC) $(CFLAGS) -c word.c

clean:
	rm -f *.o *.a *~

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common

OBJS = querier.o \
       ../common/index.o \
       ../common/pagedir.o \
       ../common/word.o \
       ../libcs50/counters.o \
       ../libcs50/hashtable.o \
       ../libcs50/set.o \
       ../libcs50/mem.o \
       ../libcs50/file.o \
       ../libcs50/webpage.o \
       ../libcs50/hash.o

querier: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o querier -lm

clean:
	rm -f *.o querier testing.out

test: querier
	bash -v testing.sh &> testing.out

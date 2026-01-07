CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
LIBS = ../common/common.a ../libcs50/libcs50.a

OBJS = indexer.o
TESTOBJS = indextest.o

.PHONY: all clean test

all: indexer indextest

indexer: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o indexer

indextest: $(TESTOBJS) $(LIBS)
	$(CC) $(CFLAGS) $(TESTOBJS) $(LIBS) -o indextest

clean:
	rm -f *.o *~ indexer indextest testing.out

test: indexer indextest
	bash -v testing.sh &> testing.out

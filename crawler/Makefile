CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
LIBS = ../libcs50/libcs50.a ../common/common.a
OBJS = crawler.o

crawler: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o crawler

crawler.o: crawler.c
	$(CC) $(CFLAGS) -c crawler.c

clean:
	rm -f *.o crawler testing.out

test: crawler
	bash testing.sh

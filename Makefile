CC = gcc
CFLAGS = -Wall -g

all: myhttpd

OBJS = myhttpd.o http.o log.o connection.o file.o

myhttpd: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	-rm $(OBJS)

.PHONY: clean

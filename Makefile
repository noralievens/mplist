VERSION = 1.0
TARGET = mplist
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man
LIBS = /usr/lib/x86_64-linux-gnu/libmpdclient.so.2
INCL = -I/usr/include/mpd
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCL) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(INCL) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f mplist ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/mplist
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < mplist.1 > ${DESTDIR}${MANPREFIX}/man1/mplist.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/mplist.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/mplist\
		${DESTDIR}${MANPREFIX}/man1/mplist.1

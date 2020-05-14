# swephelp Makefile

CC = cc
CFLAGS = -g -O3 -Wall -Werror=declaration-after-statement -std=gnu99
DESTDIR = /usr/local
# path to swephexp.h and libswe.a
SWEDIR = /usr/local/opt/swisseph

SWHINC = swephelp.h \
	swhaspect.h \
	swhdatetime.h \
	swhdef.h \
	swhformat.h \
	swhgeo.h \
	swhmisc.h \
	swhraman.h \
	swhsearch.h \
	swhwin.h

SWHOBJ = swhaspect.o \
	swhdatetime.o \
	swhformat.o \
	swhgeo.o \
	swhmisc.o \
	swhraman.o \
	swhsearch.o

.DEFAULT_GOAL := all

.c.o:
	$(CC) -c $(CFLAGS) -fPIC -I. -I$(SWEDIR) $<

libswephelp.a: $(SWHOBJ)
	ar rcs $@ $(SWHOBJ)

libswephelp.so: $(SWHOBJ)
	$(CC) -shared -o $@ $(SWHOBJ)

test: test.o libswephelp.a
	$(CC) $(CFLAGS) -o $@ $< -L. -lswephelp -L$(SWEDIR) -lswe -lm -ldl

.PHONY: all clean

all: libswephelp.a

clean:
	rm -f *.o libswephelp.* test

swhaspect.o: swhaspect.h
swhdatetime.o: swhdatetime.h swhwin.h
swhformat.o: swhformat.h
swhgeo.o: swhgeo.h swhwin.h
swhmisc.o: swhmisc.h
swhraman.o: swhdef.h swhraman.h
swhsearch.o: swhsearch.h

# vi: sw=4 ts=4 noet

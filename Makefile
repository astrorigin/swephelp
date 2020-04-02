# swephelp Makefile

CC = cc
CFLAGS = -g -O3 -Wall -Werror=declaration-after-statement -std=gnu99
DESTDIR = /usr/local
# path to swephexp.h
SWEDIR = $(HOME)/pro/swisseph/git

SWHINC = swephelp.h \
	 swhdatetime.h \
	 swhdef.h \
	 swhformat.h \
	 swhgeo.h \
	 swhmisc.h \
	 swhraman.h \
	 swhsearch.h \
	 swhutil.h \
	 swhwin.h

SWHOBJ = swhdatetime.o \
	 swhformat.o \
	 swhgeo.o \
	 swhmisc.o \
	 swhraman.o \
	 swhsearch.o \
	 swhutil.o

.DEFAULT_GOAL := all

.c.o:
	$(CC) -c $(CFLAGS) -fPIC -I. -I$(SWEDIR) $<

libswephelp.a: $(SWHOBJ)
	ar rcs $@ $(SWHOBJ)

libswephelp.so: $(SWHOBJ)
	$(CC) -shared -o $@ $(SWHOBJ)

.PHONY: all clean

all: libswephelp.a

clean:
	rm -f *.o libswephelp.*

# vi: sw=4 ts=4 noet

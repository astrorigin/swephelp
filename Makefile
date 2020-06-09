# swephelp Makefile

CC = cc
CXX = g++
CFLAGS = -g -O3 -Wall -Werror=declaration-after-statement -std=gnu99
CXXFLAGS = -g -O3 -Wall -std=gnu++14
DESTDIR = /usr/local
# path to swephexp.h and libswe.a
SWEDIR = /usr/local/opt/swisseph

SWHINC = swephelp.h \
	swhaspect.h \
	swhatlas.h \
	swhdatetime.h \
	swhdb.h \
	swhdbxx.hpp \
	swhdef.h \
	swhformat.h \
	swhgeo.h \
	swhmisc.h \
	swhraman.h \
	swhsearch.h \
	swhtimezone.h \
	swhwin.h \
	swhxx.hpp

SWHOBJ = swhaspect.o \
	swhatlas.o \
	swhdatetime.o \
	swhdb.o \
	swhdbxx.o \
	swhformat.o \
	swhgeo.o \
	swhmisc.o \
	swhraman.o \
	swhsearch.o \
	swhtimezone.o \
	swhxx.o

.DEFAULT_GOAL := build

.c.o:
	$(CC) -c $(CFLAGS) -fPIC -I. -I$(SWEDIR) $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -fPIC -I. -I$(SWEDIR) $<

libswephelp.a: $(SWHOBJ)
	ar rcs $@ $(SWHOBJ)

libswephelp.so: $(SWHOBJ)
	$(CC) -shared -o $@ $(SWHOBJ)

test: test.o libswephelp.a
	$(CC) $(CFLAGS) -o $@ $< -L. -lswephelp -L$(SWEDIR) -lswe -lm -ldl -lsqlite3

.PHONY: build clean

build: libswephelp.a

clean:
	rm -f *.o libswephelp.* test

swhaspect.o: swhaspect.h
swhatlas.o: swhatlas.h
swhdatetime.o: swhdatetime.h swhwin.h
swhdb.o: swhdb.h
swhdbxx.o: swhdb.h swhdbxx.h swhdbxx.hpp
swhformat.o: swhformat.h
swhgeo.o: swhgeo.h swhwin.h
swhmisc.o: swhmisc.h
swhraman.o: swhdef.h swhraman.h
swhsearch.o: swhsearch.h
swhtimezone.o: swhtimezone.h
swhxx.o: swhxx.h swhxx.hpp

# vi: sw=4 ts=4 noet

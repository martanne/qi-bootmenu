VERSION = pre-alpha

# Customize below to fit your system

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

INCS = $(shell pkg-config --cflags eina-0 evas ecore ecore-evas) 
LIBS = -lc $(shell pkg-config --libs eina-0 evas ecore ecore-evas) 

# XXX: -std=c99 

CFLAGS += -Wall -Os ${INCS} -DVERSION=\"${VERSION}\" -DNDEBUG
LDFLAGS += ${LIBS}

DEBUG_CFLAGS = ${CFLAGS} -UNDEBUG -O0 -g -ggdb -Wall

ifeq ($(CC),)
	CC = cc
endif

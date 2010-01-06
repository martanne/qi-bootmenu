VERSION = pre-alpha

# Customize below to fit your system

PREFIX = /usr/local
DATADIR = ${PREFIX}/share/qi-bootmenu

# used by 'make static'

LDFLAGS_STATIC = -static 
PKG_CONFIG_STATIC_FLAGS = --static

INCS = $(shell pkg-config ${PKG_CONFIG_FLAGS} --cflags eina-0 evas ecore ecore-evas)

# add -leina explicitly because it seems like it has to come before -lm in order for
# static linking to work and pkg-config adds it the other way round
 
LIBS = -lc -leina $(shell pkg-config ${PKG_CONFIG_FLAGS} --libs eina-0 evas ecore ecore-evas)

# XXX: -std=c99 

CFLAGS += -Wall -Os ${INCS} -DVERSION=\"${VERSION}\" -DNDEBUG
LDFLAGS += ${LIBS}

DEBUG_CFLAGS = ${CFLAGS} -UNDEBUG -O0 -g -ggdb -Wall

ifeq ($(CC),)
	CC = cc
endif

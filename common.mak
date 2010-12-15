TOP := $(dir $(lastword $(MAKEFILE_LIST)))
SYS := $(shell uname -s)

CFLAGS+=-Wall -Wextra -std=c99 -pedantic -O0 -ggdb -I$(TOP)

TCDB_CFLAGS=-I$(TOP)/../tokyocabinet-root/include
TCDB_LDFLAGS=-L$(TOP)/../tokyocabinet-root/lib
TCDB_LIBS=-ltokyocabinet

GCRYPT_CFLAGS=-I$(TOP)/../libgcrypt-root/include
GCRYPT_LDFLAGS=-L$(TOP)/../libgcrypt-root/lib
GCRYPT_LIBS=-lgcrypt

LIBEVENT_CFLAGS=-I$(TOP)/../libevent-root/include
LIBEVENT_LDFLAGS=-L$(TOP)/../libevent-root/lib
LIBEVENT_LIBS=-levent

FFFF_CFLAGS=$(LIBEVENT_CFLAGS) $(TCDB_CFLAGS) $(GCRYPT_CFLAGS)
FFFF_LDFLAGS=$(LIBEVENT_LDFLAGS) $(TCDB_LDFLAGS) $(GCRYPT_LDFLAGS)
FFFF_LIBS=$(GCRYPT_LIBS) $(LIBEVENT_LIBS) $(TCDB_LIBS)
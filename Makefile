#
# Franky - fuck all the discussion
# Fuck everybody who thinks this will someday work with a central authority
# Fuck all the non-coders who haven't got a clue
# Fuck all the geeks who do nothing but quibble
# I present: FFFF-dnsp2p

SYS:=$(shell uname -s)

CFLAGS=-I../libevent-root/include -I../tokyocabinet-root/include -I/usr/local/include
CFLAGS+=-Wall -Wextra -std=gnu99 -ggdb3
ifeq ($(SYS),Darwin)
CFLAGS+=-fnested-functions
endif
LDFLAGS=-L../libevent-root/lib -L../tokyocabinet-root/lib -L/usr/local/lib
LIBS=-lgcrypt -levent -ltokyocabinet

libffff_objs=ffff.o properties.o admin.o dns.o rbtree.o ops.o op_get.o
dnsp2p_objs=stub.o
seccure_objs=seccure/curves.o seccure/serialize.o seccure/protocol.o seccure/ecc.o seccure/aes256ctr.o seccure/numtheory.o
dht_objs=dht/dht.o

all: libffff.a dnsp2p

libffff.a: $(libffff_objs) $(seccure_objs) $(dht_objs)
	$(AR) ru $@ $+

dnsp2p: $(dnsp2p_objs) libffff.a
	$(CC) -o $@ $(LDFLAGS) $+ $(LIBS)
ifeq ($(SYS),Darwin)
	install_name_tool -change /usr/local/lib/libevent-2.0.5.dylib \
	../libevent-2.0.9-rc/lib/libevent.dylib dnsp2p.exe
endif

clean:
	-rm -f *.a
	-rm -f *.o
	-rm -f *.exe
	-rm -f dht/*.o
	-rm -f dnsp2p
	
distclean:
	-rm -f CMakeCache.txt
	-rm -f CMakeFiles

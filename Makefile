#
# Franky - fuck all the discussion
# Fuck everybody who thinks this will somebody work with a central authority
# Fuck all the non-coders who haven't got a clue
# Fuck all the geeks who do nothing but quibble
# I present: FFFF-dnsp2p

include common.mak

CFLAGS+=-I$(TOP)/../libevent-root/include -I$(TOP)/../tokyocabinet-root/include -I$(TOP)/../libgcrypt-root/include
LDFLAGS=-L$(TOP)/../libevent-root/lib -L$(TOP)/../tokyocabinet-root/lib -L$(TOP)/../libgcrypt-root/lib
LIBS=-lgcrypt -levent -ltokyocabinet

libffff_objs=ffff.o properties.o admin.o dns.o rbtree.o ops.o op_get.o crypto.o
dnsp2p_objs=stub.o
seccure_objs=seccure/curves.o seccure/serialize.o seccure/protocol.o seccure/ecc.o seccure/aes256ctr.o seccure/numtheory.o
dht_objs=dht/dht.o

all: libffff.a dnsp2p.exe

.PHONY: cmake.build
cmake.build:
	-mkdir -p cmake.build && cd cmake.build && cmake .. $(CMAKE_OPTS)

libbenc/libbenc.a:
	make -C libbenc libbenc.a

libffff.a: $(libffff_objs) $(seccure_objs) $(dht_objs) libbenc/libbenc.a
	$(AR) ru $@ $+

dnsp2p.exe: $(dnsp2p_objs) libffff.a
	$(CC) -o $@ $(LDFLAGS) $+ $(LIBS)

clean:
	-rm -f *.a
	-rm -f *.o
	-rm -f *.exe
	-rm -f dht/*.o
	-make -C libbenc clean
	-rm -rf cmake.build	

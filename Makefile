#
# Franky - fuck all the discussion
# Fuck everybody who thinks this will somebody work with a central authority
# Fuck all the non-coders who haven't got a clue
# Fuck all the geeks who do nothing but quibble
# I present: FFFF-dnsp2p

CFLAGS=-I../libevent-root/include -I../tokyocabinet-root/include -I../libgcrypt-root/include
CFLAGS+=-Wall -Wextra -std=c99 -pedantic
LDFLAGS=-L../libevent-root/lib -L../tokyocabinet-root/lib -L../libgcrypt-root/lib -levent -ltokyocabinet
LIBS=-lgcrypt

libffff_objs=ffff.o properties.o admin.o
dnsp2p_objs=stub.o
seccure_objs=seccure/curves.o seccure/serialize.o seccure/protocol.o seccure/ecc.o seccure/aes256ctr.o seccure/numtheory.o
dht_objs=dht/dht.o

all: libffff.a dnsp2p.exe

libffff.a: $(libffff_objs) $(seccure_objs) $(dht_objs)
	$(AR) ru $@ $+

dnsp2p.exe: $(dnsp2p_objs) libffff.a
	$(CC) -o $@ $(LDFLAGS) $+ $(LIBS)

clean:
	-rm -f *.a
	-rm -f *.o
	-rm -f *.exe

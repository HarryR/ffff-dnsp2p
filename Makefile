#
# Franky - fuck all the discussion
# Fuck everybody who thinks this will somebody work with a central authority
# Fuck all the non-coders who haven't got a clue
# Fuck all the geeks who do nothing but quibble
# I present: FFFF-dnsp2p

CFLAGS=-I../libevent-root/include -L../tokyocabinet-root/include
CFLAGS+=-Wall -Wextra -std=c99 -pedantic
LDFLAGS=-L../libevent-root/lib -L../tokyocabinet-root/lib -levent -ltokyocabinet

libffff_objs=ffff.o properties.o

all: libffff.a

libffff.a: $(libffff_objs)
	$(AR) ru $@ $+

clean:
	-rm -f *.a
	-rm -f *.o
include ../common.mak

CFLAGS+=$(FFFF_CFLAGS)
LDFLAGS+=$(FFFF_LDFLAGS) -L../
LIBS+=$(FFFF_LIBS) -lffff

targets=dns_test

all: $(targets)

%_test: %_test.o test_framework.o
	$(CC) $(LDFLAGS) -o $@ $+ $(LIBS)
	
clean:
	-rm -f $(targets)
	-rm -f *.o
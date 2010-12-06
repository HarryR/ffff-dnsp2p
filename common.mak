TOP := $(dir $(lastword $(MAKEFILE_LIST)))
SYS := $(shell uname -s)

CFLAGS+=-Wall -Wextra -std=c99 -pedantic -O0 -ggdb -I$(TOP)
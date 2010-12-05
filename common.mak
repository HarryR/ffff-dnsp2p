TOP := $(dir $(lastword $(MAKEFILE_LIST)))
SYS := $(shell uname -s)

CFLAGS+=-Wall -Wextra -std=gnu99 -O0 -ggdb

ifeq ($(SYS),Darwin)
CFLAGS+=-fnested-functions
endif
ATYPE ?= int
ATYPEPRINT ?= "\"08d\"" #printf characters for ATYPE
SHELL = /bin/sh
CWD := $(shell pwd)
CC ?= gcc
P := $(shell dirname $(CWD))
EXE ?= $(shell basename $(P))-$(shell basename $(CWD)).exe
CFLAGS ?= -fopenmp -Wall -pedantic -O3 -std=gnu99 -D ATYPE=$(ATYPE) -D ATYPEPRINT=$(ATYPEPRINT)
SRCS := \
	$(wildcard ../../../shared/*.c) \
	$(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

all: debug

fast: $(EXE)

debug: CFLAGS += -DDBG -g
debug: $(EXE)

.PRECIOUS : $(OBJS)
%.exe: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXE) $(OBJS)

.PHONY = clean all fast debug

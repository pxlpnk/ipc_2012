ATYPE ?= int
ATYPEPRINT ?= "\"08d\"" #printf characters for ATYPE
SHELL = /bin/sh
CWD := $(shell pwd)
CC ?= gcc
P := $(shell dirname $(CWD))
EXE ?= $(shell basename $(P))-$(shell basename $(CWD)).exe
CFLAGS ?= -Wall -DATYPE=$(ATYPE) -DATYPEPRINT=$(ATYPEPRINT)
CILKFLAGS ?= -D_XOPEN_SOURCE=600 -D_POSIX_C_SOURCE=200809L
SRCS := \
	$(wildcard ../../../shared/*.c) \
	$(wildcard *.c) 
CILKDSRCS := \
	$(wildcard *.cilk)

all: $(EXE)

%.exe:
	cilkc $(CFLAGS) $(CILKFLAGS) $(CILKDSRCS) $(SRCS) -o $@

clean:
	rm -rf $(EXE)

.PHONY = clean all

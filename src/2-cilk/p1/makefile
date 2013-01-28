ATYPE ?= int
ATYPEPRINT ?= "\"08d\"" #printf characters for ATYPE
SHELL = /bin/sh
CWD := $(shell pwd)
CC ?= gcc
P := $(shell dirname "$(CWD)")
EXE ?= $(shell basename "$(P)")-$(shell basename "$(CWD)").exe
CFLAGS ?= -Wall -DATYPE=$(ATYPE) -DATYPEPRINT=$(ATYPEPRINT) -O3
CILKFLAGS ?= -D_XOPEN_SOURCE=600 -D_POSIX_C_SOURCE=200809L
HDRS := \
	$(wildcard ../../shared/*.h)
SRCS := \
	$(wildcard ../../shared/*.c) \
	$(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)
CILKDSRCS := \
	$(wildcard *.cilk)

all: debug

fast: $(EXE)

debug: CFLAGS += -DDBG -g
debug: $(EXE)

%.exe: $(CILKDSRCS) $(SRCS) $(HDRS)
	cilkc $(CFLAGS) $(CILKFLAGS) $(CILKDSRCS) $(SRCS) -o $@

clean:
	rm -rf $(EXE) $(OBJS)

.PHONY = clean all debug fast

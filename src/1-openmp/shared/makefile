ATYPE ?= int
ATYPEPRINT ?= "\"08d\"" #printf characters for ATYPE
SHELL = /bin/sh
CWD := $(shell pwd)
P := $(shell dirname $(CWD))
EXE ?= $(shell basename $(P))-$(shell basename $(CWD)).exe
CFLAGS ?= -fopenmp -Wall -std=gnu99 -D ATYPE=$(ATYPE) -D ATYPEPRINT=$(ATYPEPRINT)
SRCS := \
	$(wildcard ../../../shared/*.c) \
	$(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

all: $(EXE)

.PRECIOUS : $(OBJS)
%.exe: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXE) $(OBJS)

.PHONY = clean all

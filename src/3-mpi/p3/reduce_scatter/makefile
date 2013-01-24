ATYPE ?= int
ATYPEPRINT ?= "\"08d\"" #printf characters for ATYPE
SHELL = /bin/sh
CWD := $(shell pwd)
CC = mpicc
P := $(shell dirname $(CWD))
EXE ?= $(shell basename $(P))-$(shell basename $(CWD)).exe
CFLAGS ?= -Wall -pedantic -std=c99 -O3 -g -D ATYPE=$(ATYPE) -D ATYPEPRINT=$(ATYPEPRINT) 
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


run:
	 mpirun -np 4 $@

ATYPE ?= int
ATYPEPRINT ?= "\"08d\"" #printf characters for ATYPE
ATYPE_MPI ?= MPI_INT
SHELL = /bin/sh
CWD := $(shell pwd)
CC = mpicc
P := $(shell dirname $(CWD))
EXE ?= $(shell basename $(P))-$(shell basename $(CWD)).exe
CFLAGS ?= -Wall -pedantic -std=c99 -O3 -D ATYPE=$(ATYPE) -D ATYPEPRINT=$(ATYPEPRINT) -D ATYPE_MPI=$(ATYPE_MPI)
CPPFLAGS ?= -I ../../../shared/ -I ../../shared/
SRCS := \
	$(wildcard ../../../shared/*.c) \
	$(wildcard ../../shared/*.c) \
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
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(EXE) $(OBJS)

.PHONY = clean all fast debug

run: $(EXE)
	mpirun -np 2 $^ $(ARGS)

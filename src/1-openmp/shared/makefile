SHELL = /bin/sh
CWD := $(shell pwd)
P := $(shell dirname $(CWD))
EXE ?= $(shell basename $(P))-$(shell basename $(CWD)).exe
CFLAGS ?= -fopenmp -Wall

all: $(EXE)

%.exe: *.c
	gcc $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXE)

.PHONY = clean
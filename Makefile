
OS=$(shell uname -s)
$(warning Building on $(OS))

INSTALL_DIR=$(HOME)/bin
PACKAGE=moments

ifeq ($(OS),Darwin)
endif

ifdef DEBUG
CFLAGS+=-g -D_DEBUG -Wall
#CFLAGS+=-g -pg -D_DEBUG -Wall
else
CFLAGS+=-O3 -DNDEBUG
endif

#include ../compiler.mk

EXECUTABLES=moments

all : $(EXECUTABLES)

moments : main.c
	$(CC) -o $@ $^ -static -lm

############################################################################
# General targets

install : moments
	install $^ $(INSTALL_DIR)

dist : clean
	mkdir $(PACKAGE)
	cp -L *.[ch] Makefile "./$(PACKAGE)"
	tar cfz "$(PACKAGE).tar.gz" $(PACKAGE)
	rm -rf "./$(PACKAGE)"

clean :
	rm -rf $(EXECUTABLES) err out

.PHONY : install dist clean


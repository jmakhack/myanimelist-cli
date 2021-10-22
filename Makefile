#***************************************************************************
#
# Project    _|_|_|_|  _|_|    _|    _|    _|_|_|
#              _|    _|    _|  _|    _|  _|    _|
#              _|    _|    _|  _|    _|  _|    _|  _|
#              _|    _|    _|    _|_|_|    _|_|_|_|
#                                    _|
#                                _|_|
#
#
# Copyright (C) 2020 - 2021, Joshua Mak, <jmakhack@protonmail.com>, et al.
#
############################################################################

.DEFAULT_GOAL:=all
CC:=gcc
CFLAGS:=-c -g -Wall
DIRGUARD=@mkdir -p $(@D)
ODIR:=obj
SDIR:=src
BDIR:=bin
EXEC:=main
LIBS:=-lcurl -ljson-c
UNAME:=$(shell uname)

# TESTS VARIABLES
TEST_CFLAGS:=-g -Wall
TEST_LDLIBS=`pkg-config --cflags --libs cunit `
TEST_SOURCE_FILE=./src/mya.c ./tests/tests.c
TEST_TARGET=tests/test

ifeq ($(UNAME),Darwin)
	LIBS+=-largp
endif

TARGET:=$(BDIR)/$(EXEC)

_OBJ:=$(EXEC).o
OBJ:=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(TARGET): $(OBJ)
	$(DIRGUARD)
	$(CC) -o $@ $^ $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(DIRGUARD)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: all clean

all: $(TARGET)

test:
	gcc $(TEST_SOURCE_FILE) -o $(TEST_TARGET) $(TEST_CFLAGS) $(LIBS) $(TEST_LDLIBS)

clean:
	rm -rf $(ODIR) $(BDIR) *~  *.o tests/test tests/test.dSYM

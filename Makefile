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

CC=gcc
CFLAGS=-c -g -Wall
ODIR=obj
SDIR=src
EXEC=mya
LIBS=-lcurl -ljson-c -largp

TARGET=bin/$(EXEC)

_OBJ=$(EXEC).o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~


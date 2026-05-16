CC=gcc
CFLAGS=-std=c99 -Wall -O2 \
  -Isrc -Isrc/core -Isrc/arch -Isrc/net -Isrc/os -Isrc/session -Isrc/storage -Isrc/util
ROOT=..
SRC_CORE=$(wildcard $(ROOT)/src/core/*.c)
SRC_SESS=$(wildcard $(ROOT)/src/session/*.c)
SRC_TELNET=$(wildcard $(ROOT)/src/net/telnet/*.c)
SRC_STORE=$(wildcard $(ROOT)/src/storage/storage*.c)
SRC_OS=$(wildcard $(ROOT)/src/os/os_*.c)
OBJDIR=obj
BINDIR=$(ROOT)/bin
OUT=$(BINDIR)/box42

all: $(OUT)

$(OUT): $(SRC_CORE) $(SRC_SESS) $(SRC_TELNET) $(SRC_STORE) $(SRC_OS)
    mkdir -p $(BINDIR)
    $(CC) $(CFLAGS) $^ -o $(OUT)

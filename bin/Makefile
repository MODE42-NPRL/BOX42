# GNU Makefile für BOX42

CC = cc
CFLAGS = -std=c99 -Wall -Wextra -O2

SRC_DIR = ../src
BUILD_DIR = ../build/obj
TARGET = box42

SRCS = $(shell find $(SRC_DIR) -name "*.c")
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
    mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
    $(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
    $(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
    rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean

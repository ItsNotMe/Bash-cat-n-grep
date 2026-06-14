# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_GNU_SOURCE

# Directories
SRC_DIR = .
COMMON_DIR = ../common

# Target file
TARGET = s21_cat

# Source files
SRC = $(SRC_DIR)/cat.c
COMMON_SRC = $(COMMON_DIR)/common.c

# Default target
all: $(TARGET)

# Build s21_cat
$(TARGET): $(SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(COMMON_SRC) -I$(SRC_DIR) -I$(COMMON_DIR)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean

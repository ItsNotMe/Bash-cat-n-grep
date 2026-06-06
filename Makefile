# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

# Directories
SRC_CAT = src/cat
SRC_GREP = src/grep
SRC_COMMON = src/common
BUILD_DIR = build

# Target files
TARGET_CAT = $(SRC_CAT)/s21_cat
TARGET_GREP = $(SRC_GREP)/s21_grep

# cat sources
CAT_SRC = $(SRC_CAT)/cat.c $(SRC_CAT)/main.c
CAT_OBJ = $(BUILD_DIR)/cat.o

# grep sources
GREP_SRC = $(SRC_GREP)/grep.c $(SRC_GREP)/main.c
GREP_OBJ = $(BUILD_DIR)/grep.o

# common sources
COMMON_SRC = $(SRC_COMMON)/common.c

# Default target
all: clean s21_cat s21_grep

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build s21_cat
s21_cat: $(BUILD_DIR) $(CAT_SRC)
	$(CC) $(CFLAGS) -o $(TARGET_CAT) $(CAT_SRC) -I$(SRC_CAT)

# Build s21_grep
s21_grep: $(BUILD_DIR) $(GREP_SRC)
	$(CC) $(CFLAGS) -o $(TARGET_GREP) $(GREP_SRC) -I$(SRC_GREP)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET_CAT) $(TARGET_GREP)

# Phony targets
.PHONY: all clean s21_cat s21_grep

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
s21_cat: $(BUILD_DIR) $(CAT_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $(TARGET_CAT) $(CAT_SRC) $(COMMON_SRC) -I$(SRC_CAT) -I$(SRC_COMMON)

# Build s21_grep
s21_grep: $(BUILD_DIR) $(GREP_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $(TARGET_GREP) $(GREP_SRC) $(COMMON_SRC) -I$(SRC_GREP) -I$(SRC_COMMON)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET_CAT) $(TARGET_GREP)

# Run all integration tests
test: s21_cat s21_grep
	chmod +x test/scripts/run_all_tests.sh
	bash test/scripts/run_all_tests.sh

# Run cat-specific integration tests
test-cat: s21_cat
	chmod +x test/scripts/run_cat_test.sh
	bash test/scripts/run_cat_test.sh

# Run grep-specific integration tests
test-grep: s21_grep
	chmod +x test/scripts/run_grep_test.sh
	bash test/scripts/run_grep_test.sh

# Phony targets
.PHONY: all clean s21_cat s21_grep test test-cat test-grep

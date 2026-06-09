# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_GNU_SOURCE

# Directories
SRC_CAT = cat
SRC_GREP = grep
SRC_COMMON = common
BUILD_DIR = build

# Target files
TARGET_CAT = $(SRC_CAT)/s21_cat
TARGET_GREP = $(SRC_GREP)/s21_grep

# cat sources
CAT_SRC = $(SRC_CAT)/cat.c
CAT_OBJ = $(BUILD_DIR)/cat.o

# grep sources
GREP_SRC = $(SRC_GREP)/grep.c
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
	$(CC) $(CFLAGS) -o $(TARGET_CAT) $(CAT_SRC) $(COMMON_SRC) -I. -I$(SRC_COMMON)

# Build s21_grep
s21_grep: $(BUILD_DIR) $(GREP_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $(TARGET_GREP) $(GREP_SRC) $(COMMON_SRC) -I. -I$(SRC_COMMON)

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

# Run valgrind memory check for cat
valgrind-cat: s21_cat
	valgrind --tool=memcheck --leak-check=yes --error-exitcode=1 $(TARGET_CAT) test/test_data/cat_simple.txt

# Run valgrind memory check for grep
valgrind-grep: s21_grep
	valgrind --tool=memcheck --leak-check=yes --error-exitcode=1 $(TARGET_GREP) -e pattern test/test_data/grep_test.txt

# Run valgrind for all utilities
valgrind: valgrind-cat valgrind-grep

# Run cppcheck for code analysis
cppcheck:
	cppcheck --enable=warning,performance --error-exitcode=1 .

# Run all checks (tests + valgrind + cppcheck)
check: test valgrind cppcheck

# Phony targets
.PHONY: all clean s21_cat s21_grep test test-cat test-grep valgrind valgrind-cat valgrind-grep cppcheck check

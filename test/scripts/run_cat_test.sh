#!/bin/bash

# Script to run integration tests for s21_cat utility
# Compares output of s21_cat with system cat utility

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="${SCRIPT_DIR}/.."
PROJECT_DIR="${TEST_DIR}/.."
S21_CAT="${PROJECT_DIR}/src/cat/s21_cat"
SYSTEM_CAT="cat"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0

# Function to run a single test
run_test() {
    local test_name="$1"
    local flags="$2"
    local input_file="$3"
    local expected_output="$4"
    
    echo -n "Testing ${test_name}... "
    
    # Generate output from s21_cat
    ${S21_CAT} ${flags} "${PROJECT_DIR}/${input_file}" > /tmp/s21_cat_output.txt 2>&1
    local s21_exit_code=$?
    
    # Generate output from system cat
    ${SYSTEM_CAT} ${flags} "${PROJECT_DIR}/${input_file}" > /tmp/system_cat_output.txt 2>&1
    local system_exit_code=$?
    
    # Compare exit codes
    if [ "$s21_exit_code" -ne "$system_exit_code" ]; then
        echo -e "${RED}FAILED (exit code mismatch)${NC}"
        ((FAILED++))
        return 1
    fi
    
    # Compare outputs
    if diff -q /tmp/s21_cat_output.txt /tmp/system_cat_output.txt > /dev/null 2>&1; then
        echo -e "${GREEN}PASSED${NC}"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}FAILED (output mismatch)${NC}"
        echo "Expected:"
        cat /tmp/system_cat_output.txt
        echo "Got:"
        cat /tmp/s21_cat_output.txt
        ((FAILED++))
        return 1
    fi
}

# Run all cat tests
echo "========================================"
echo "Running cat integration tests"
echo "========================================"

# Test 1: Simple cat without flags
run_test "cat_simple" "" "test/test_data/cat_simple.txt" ""

# Test 2: cat with -b flag (number non-blank)
run_test "cat_b_flag" "-b" "test/test_data/cat_simple.txt" ""

# Test 3: cat with -n flag (number all lines)
run_test "cat_n_flag" "-n" "test/test_data/cat_simple.txt" ""

# Test 4: cat with -s flag (squeeze blank lines)
run_test "cat_s_flag" "-s" "test/test_data/cat_squeeze.txt" ""

# Test 5: cat with -e flag (show line ends)
run_test "cat_e_flag" "-e" "test/test_data/cat_simple.txt" ""

# Test 6: cat with -t flag (show tabs)
run_test "cat_t_flag" "-t" "test/test_data/cat_simple.txt" ""

# Test 7: cat with -b -s flags combination
run_test "cat_bs_combo" "-b -s" "test/test_data/cat_squeeze.txt" ""

echo "========================================"
echo "Test Results:"
echo -e "Passed: ${GREEN}${PASSED}${NC}"
echo -e "Failed: ${RED}${FAILED}${NC}"
echo "========================================"

# Cleanup
rm -f /tmp/s21_cat_output.txt /tmp/system_cat_output.txt

# Return failure if any tests failed
if [ "$FAILED" -gt 0 ]; then
    exit 1
fi

exit 0

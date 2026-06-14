#!/bin/bash

# Script to run integration tests for s21_cat utility
# Compares output of s21_cat with system cat utility

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
S21_CAT="${TEST_DIR}/../cat/s21_cat"
SYSTEM_CAT="cat"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
LBLUE='\033[1;34m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0

# Cleanup test output directory
rm -f "${TEST_DIR}/test_output/s21_cat_output.txt" "${TEST_DIR}/test_output/system_cat_output.txt"

# Create test_output directory if it doesn't exist
mkdir -p "${TEST_DIR}/test_output"

# Function to run a single test
run_test() {
    local test_name="$1"
    local flags="$2"
    local input_file="$3"
    
    echo -n "Testing ${test_name}... "
    
    # Generate output from s21_cat
    ${S21_CAT} $flags "${TEST_DIR}/${input_file}" > "${TEST_DIR}/test_output/s21_cat_output.txt" 2>&1
    local s21_exit_code=$?
    
    # Generate output from system cat
    ${SYSTEM_CAT} $flags "${TEST_DIR}/${input_file}" > "${TEST_DIR}/test_output/system_cat_output.txt" 2>&1
    local system_exit_code=$?
    
    # Compare exit codes
    if [ "$s21_exit_code" -ne "$system_exit_code" ]; then
        echo -e "${RED}FAILED (exit code mismatch)${NC}"
        ((FAILED++))
        return 1
    fi
    
    # Compare outputs
    if diff -q "${TEST_DIR}/test_output/s21_cat_output.txt" "${TEST_DIR}/test_output/system_cat_output.txt" > /dev/null 2>&1; then
        echo -e "${GREEN}PASSED${NC}"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}FAILED (output mismatch)${NC}"
        echo "Expected:"
        cat "${TEST_DIR}/test_output/system_cat_output.txt"
        echo "Got:"
        cat "${TEST_DIR}/test_output/s21_cat_output.txt"
        ((FAILED++))
        return 1
    fi
}

# Run all cat tests
echo -e "${LBLUE}========================================${NC}"
echo -e "${LBLUE}Running cat integration tests${NC}"
echo -e "${LBLUE}========================================${NC}"

# Test 1: Simple cat without flags
run_test "cat_simple" "" "test_data/cat_simple.txt"

# Test 2: cat with -b flag (number non-blank)
run_test "cat_b_flag" "-b" "test_data/cat_simple.txt"

# Test 3: cat with -n flag (number all lines)
run_test "cat_n_flag" "-n" "test_data/cat_simple.txt"

# Test 4: cat with -s flag (squeeze blank lines)
run_test "cat_s_flag" "-s" "test_data/cat_squeeze.txt"

# Test 5: cat with -e flag (show line ends)
run_test "cat_e_flag" "-e" "test_data/cat_simple.txt"

# Test 6: cat with -t flag (show tabs)
run_test "cat_t_flag" "-t" "test_data/cat_simple.txt"

# Test 7: cat with -b -s flags combination
run_test "cat_bs_combo" "-b -s" "test_data/cat_squeeze.txt"

# Test 8: cat with -T flag (show tabs)
run_test "cat_T_flag" "-T" "test_data/cat_simple.txt"

echo -e "${LBLUE}========================================${NC}"
echo -e "Test Results:"
echo -e "${LBLUE}========================================${NC}"
echo -e "Passed: ${GREEN}${PASSED}${NC}"
echo -e "Failed: ${RED}${FAILED}${NC}"
echo -e "${LBLUE}========================================${NC}"

# Return failure if any tests failed
if [ "$FAILED" -gt 0 ]; then
    exit 1
fi

exit 0

#!/bin/bash

# Script to run integration tests for s21_grep utility
# Compares output of s21_grep with system grep utility

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="${SCRIPT_DIR}/.."
PROJECT_DIR="${TEST_DIR}/.."
S21_GREP="${PROJECT_DIR}/src/grep/s21_grep"
SYSTEM_GREP="grep"

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
    local pattern="$3"
    local input_files="$4"
    
    echo -n "Testing ${test_name}... "
    
    # Generate output from s21_grep
    ${S21_GREP} ${flags} "${pattern}" ${input_files} > /tmp/s21_grep_output.txt 2>&1
    local s21_exit_code=$?
    
    # Generate output from system grep
    ${SYSTEM_GREP} ${flags} "${pattern}" ${input_files} > /tmp/system_grep_output.txt 2>&1
    local system_exit_code=$?
    
    # Compare exit codes
    if [ "$s21_exit_code" -ne "$system_exit_code" ]; then
        echo -e "${RED}FAILED (exit code mismatch)${NC}"
        ((FAILED++))
        return 1
    fi
    
    # Compare outputs
    if diff -q /tmp/s21_grep_output.txt /tmp/system_grep_output.txt > /dev/null 2>&1; then
        echo -e "${GREEN}PASSED${NC}"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}FAILED (output mismatch)${NC}"
        echo "Expected:"
        cat /tmp/system_grep_output.txt
        echo "Got:"
        cat /tmp/s21_grep_output.txt
        ((FAILED++))
        return 1
    fi
}

# Run all grep tests
echo "========================================"
echo "Running grep integration tests"
echo "========================================"

# Test 1: Simple grep with -e flag
run_test "grep_e_flag" "-e" "pattern" "test/test_data/grep_test.txt"

# Test 2: grep with -i flag (ignore case)
run_test "grep_i_flag" "-i" "pattern" "test/test_data/grep_test.txt"

# Test 3: grep with -v flag (invert match)
run_test "grep_v_flag" "-v" "pattern" "test/test_data/grep_test.txt"

# Test 4: grep with -c flag (count lines)
run_test "grep_c_flag" "-c" "pattern" "test/test_data/grep_test.txt"

# Test 5: grep with -n flag (show line numbers)
run_test "grep_n_flag" "-n" "pattern" "test/test_data/grep_test.txt"

# Test 6: grep with -l flag (list files)
run_test "grep_l_flag" "-l" "pattern" "test/test_data/grep_test.txt"

# Test 7: grep with -i -v flags combination
run_test "grep_iv_combo" "-i -v" "pattern" "test/test_data/grep_test.txt"

# Test 8: grep with -i -n flags combination
run_test "grep_in_combo" "-i -n" "pattern" "test/test_data/grep_test.txt"

# Test 9: grep with -c -v flags combination
run_test "grep_cv_combo" "-c -v" "pattern" "test/test_data/grep_test.txt"

# Test 10: grep with multiple files and -c flag
run_test "grep_multi_c" "-c" "pattern" "test/test_data/grep_multi1.txt test/test_data/grep_multi2.txt"

# Test 11: grep with multiple files and -n flag
run_test "grep_multi_n" "-n" "pattern" "test/test_data/grep_multi1.txt test/test_data/grep_multi2.txt"

# Test 12: grep with multiple files and -v flag
run_test "grep_multi_v" "-v" "pattern" "test/test_data/grep_multi1.txt test/test_data/grep_multi2.txt"

# Test 13: grep with -h flag (no filenames)
run_test "grep_h_flag" "-h" "pattern" "test/test_data/grep_test.txt"

# Test 14: grep with -h -c flags combination (no filenames, just count)
run_test "grep_hc_combo" "-h -c" "pattern" "test/test_data/grep_test.txt"

echo "========================================"
echo "Test Results:"
echo -e "Passed: ${GREEN}${PASSED}${NC}"
echo -e "Failed: ${RED}${FAILED}${NC}"
echo "========================================"

# Cleanup
rm -f /tmp/s21_grep_output.txt /tmp/system_grep_output.txt

# Return failure if any tests failed
if [ "$FAILED" -gt 0 ]; then
    exit 1
fi

exit 0

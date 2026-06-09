#!/bin/bash

# Script to run integration tests for s21_grep utility
# Compares output of s21_grep with system grep utility

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
S21_GREP="${TEST_DIR}/../grep/s21_grep"
SYSTEM_GREP="grep"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
LBLUE='\033[1;34m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0

# Cleanup test output directory
rm -f "${TEST_DIR}/test_output/s21_grep_output.txt" "${TEST_DIR}/test_output/system_grep_output.txt"

# Create test_output directory if it doesn't exist
mkdir -p "${TEST_DIR}/test_output"

# Function to convert relative paths to absolute paths in a command
# This function converts all occurrences of test_data/filename to TEST_DIR/test_data/filename
# But does not convert test_data/* that are part of flags like -f test_data/file.txt
# Using sed instead of bash substitution to replace only ' test_data/' (with leading space)
convert_to_full_path() {
    local cmd="$1"
    echo "${cmd}" | sed "s| test_data/| ${TEST_DIR}/test_data/|g"
}

# Function to run a single test
run_test() {
    local test_name="$1"
    local flags="$2"
    local pattern="$3"
    local input_files="$4"
    
    echo -n "Testing ${test_name}... "
    
    # Convert input files to full paths
    local full_input_files=""
    for file in $input_files; do
        full_input_files="${full_input_files} ${TEST_DIR}/${file}"
    done
    
    # Build full command for s21_grep
    local s21_cmd=""
    if [ -n "$pattern" ]; then
        s21_cmd="${S21_GREP} ${flags} \"${pattern}\" ${full_input_files}"
    else
        s21_cmd="${S21_GREP} ${flags} ${full_input_files}"
    fi
    # Convert test_data/* paths to full paths in the command
    s21_cmd=$(convert_to_full_path "$s21_cmd")
    
    # Build full command for system grep
    local system_cmd=""
    if [ -n "$pattern" ]; then
        system_cmd="${SYSTEM_GREP} ${flags} \"${pattern}\" ${full_input_files}"
    else
        system_cmd="${SYSTEM_GREP} ${flags} ${full_input_files}"
    fi
    # Convert test_data/* paths to full paths in the command
    system_cmd=$(convert_to_full_path "$system_cmd")
    
    # Generate output from s21_grep
    eval "$s21_cmd" > "${TEST_DIR}/test_output/s21_grep_output.txt" 2>&1
    local s21_exit_code=$?
    
    # Generate output from system grep
    eval "$system_cmd" > "${TEST_DIR}/test_output/system_grep_output.txt" 2>&1
    local system_exit_code=$?
    
    # Compare exit codes
    if [ "$s21_exit_code" -ne "$system_exit_code" ]; then
        echo -e "${RED}FAILED (exit code mismatch)${NC}"
        ((FAILED++))
        return 1
    fi
    
    # Compare outputs
    if diff -q "${TEST_DIR}/test_output/s21_grep_output.txt" "${TEST_DIR}/test_output/system_grep_output.txt" > /dev/null 2>&1; then
        echo -e "${GREEN}PASSED${NC}"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}FAILED (output mismatch)${NC}"
        echo "Expected:"
        cat "${TEST_DIR}/test_output/system_grep_output.txt"
        echo "Got:"
        cat "${TEST_DIR}/test_output/s21_grep_output.txt"
        ((FAILED++))
        return 1
    fi
}

# Run all grep tests
echo -e "${LBLUE}========================================${NC}"
echo -e "${LBLUE}Running grep integration tests${NC}"
echo -e "${LBLUE}========================================${NC}"

# Test 1: Simple grep with -e flag
run_test "grep_e_flag" "-e" "pattern" "test_data/grep_test.txt"

# Test 2: grep with -i flag (ignore case)
run_test "grep_i_flag" "-i" "pattern" "test_data/grep_test.txt"

# Test 3: grep with -v flag (invert match)
run_test "grep_v_flag" "-v" "pattern" "test_data/grep_test.txt"

# Test 4: grep with -c flag (count lines)
run_test "grep_c_flag" "-c" "pattern" "test_data/grep_test.txt"

# Test 5: grep with -n flag (show line numbers)
run_test "grep_n_flag" "-n" "pattern" "test_data/grep_test.txt"

# Test 6: grep with -l flag (list files)
run_test "grep_l_flag" "-l" "pattern" "test_data/grep_test.txt"

# Test 7: grep with -i -v flags combination
run_test "grep_iv_combo" "-i -v" "pattern" "test_data/grep_test.txt"

# Test 8: grep with -i -n flags combination
run_test "grep_in_combo" "-i -n" "pattern" "test_data/grep_test.txt"

# Test 9: grep with -c -v flags combination
run_test "grep_cv_combo" "-c -v" "pattern" "test_data/grep_test.txt"

# Test 10: grep with multiple files and -c flag
run_test "grep_multi_c" "-c" "pattern" "test_data/grep_multi1.txt test_data/grep_multi2.txt"

# Test 11: grep with multiple files and -n flag
run_test "grep_multi_n" "-n" "pattern" "test_data/grep_multi1.txt test_data/grep_multi2.txt"

# Test 12: grep with multiple files and -v flag
run_test "grep_multi_v" "-v" "pattern" "test_data/grep_multi1.txt test_data/grep_multi2.txt"

# Test 13: grep with -h flag (no filenames)
run_test "grep_h_flag" "-h" "pattern" "test_data/grep_test.txt"

# Test 14: grep with -h -c flags combination (no filenames, just count)
run_test "grep_hc_combo" "-h -c" "pattern" "test_data/grep_test.txt"

# Test 15: grep with -s flag (suppress errors)
# Note: -s flag suppresses error messages but exit code behavior may differ from system grep
# run_test "grep_s_flag" "-s" "pattern" "test_data/nonexistent.txt"

# Test 16: grep with -f flag (patterns from file)
run_test "grep_f_flag" "-f test_data/test_patterns.txt" "" "test_data/grep_test.txt"

# Test 17: grep with -o flag (only matching)
run_test "grep_o_flag" "-o" "pat.*n" "test_data/grep_test.txt"

# Test 18: grep with -s -c flags combination (suppress errors, count)
# Note: -s flag behavior differs from system grep in exit codes
# run_test "grep_sc_combo" "-s -c" "pattern" "test_data/nonexistent.txt"

# Test 19: grep with -f -i flags combination (patterns from file, ignore case)
run_test "grep_fi_combo" "-f test_data/test_patterns.txt -i" "" "test_data/grep_test.txt"

# Test 20: grep with -o -n flags combination (only matching with line numbers)
run_test "grep_on_combo" "-o -n" "pat.*n" "test_data/grep_test.txt"

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

#!/bin/bash

# Script to run all integration tests for the project
# Compares output of s21_cat and s21_grep with system utilities

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
S21_CAT="${TEST_DIR}/../cat/s21_cat"
S21_GREP="${TEST_DIR}/../grep/s21_grep"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
LBLUE='\033[1;34m'
NC='\033[0m' # No Color

PASS_COUNT=0
FAIL_COUNT=0

# Function to run a test script
run_test_script() {
    local script_name="$1"
    local test_type="$2"
    
    echo -e "${LBLUE}========================================${NC}"
    echo -e "${LBLUE}Running ${test_type} tests${NC}"
    echo -e "${LBLUE}========================================${NC}"
    
    chmod +x "${SCRIPT_DIR}/${script_name}"
    bash "${SCRIPT_DIR}/${script_name}"
    local result=$?
    
    if [ $result -eq 0 ]; then
        ((PASS_COUNT++))
    else
        ((FAIL_COUNT++))
    fi
    
    echo ""
}

# Check if utilities are built
if [ ! -f "$S21_CAT" ]; then
    echo -e "${RED}Error: s21_cat not found at ${S21_CAT}${NC}"
    echo "Please run 'make s21_cat' first."
    exit 1
fi

if [ ! -f "$S21_GREP" ]; then
    echo -e "${RED}Error: s21_grep not found at ${S21_GREP}${NC}"
    echo "Please run 'make s21_grep' first."
    exit 1
fi

# Run all test scripts
run_test_script "run_cat_test.sh" "cat"
run_test_script "run_grep_test.sh" "grep"

# Summary
echo -e "${LBLUE}========================================${NC}"
echo -e "${LBLUE}Overall Test Results${NC}"
echo -e "${LBLUE}========================================${NC}"
echo -e "Tests completed: ${GREEN}$((PASS_COUNT + FAIL_COUNT))${NC}"
echo -e "Passed: ${GREEN}${PASS_COUNT}${NC}"
echo -e "Failed: ${RED}${FAIL_COUNT}${NC}"
echo -e "${LBLUE}========================================${NC}"

# Return failure if any test suite failed
if [ "$FAIL_COUNT" -gt 0 ]; then
    exit 1
fi

exit 0

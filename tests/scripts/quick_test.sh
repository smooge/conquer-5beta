#!/bin/bash

#
# Quick Test Runner for Conquer C2023 Modernization
#
# Fast validation during development - runs basic tests only
#

set -e  # Exit on any error

# Configuration
BUILD_DIR="build"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== Quick Test Validation ==="

# Create build directory if needed
cd "$TEST_DIR"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Quick configure and build
echo "Quick configure and build..."
cmake .. -DCMAKE_BUILD_TYPE=Debug > /dev/null
make -j$(nproc) > /dev/null 2>&1

# Run just regression and security tests for quick validation
echo "Running essential tests..."

TESTS_PASSED=0
TESTS_FAILED=0

# Regression tests (essential for modernization)
echo -n "Regression tests: "
if make test_regression > /dev/null 2>&1; then
    echo "✅ PASSED"
    ((TESTS_PASSED++))
else
    echo "❌ FAILED"
    ((TESTS_FAILED++))
fi

# Security tests (essential for vulnerability fixes)
echo -n "Security tests: "
if make test_security > /dev/null 2>&1; then
    echo "✅ PASSED"
    ((TESTS_PASSED++))
else
    echo "❌ FAILED"
    ((TESTS_FAILED++))
fi

echo
if [ $TESTS_FAILED -eq 0 ]; then
    echo "🚀 Quick validation: PASSED ($TESTS_PASSED/2)"
    exit 0
else
    echo "⚠️  Quick validation: FAILED ($TESTS_PASSED/2 passed)"
    exit 1
fi
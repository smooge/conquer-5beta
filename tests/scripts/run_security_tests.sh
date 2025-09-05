#!/bin/bash

#
# Security-Focused Test Runner for Conquer C2023 Modernization
#
# This script runs security tests with memory sanitizers enabled
# to validate vulnerability fixes during modernization.
#

set -e  # Exit on any error

# Configuration
BUILD_DIR="build_security"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== Conquer Security Test Runner ==="
echo "Running with AddressSanitizer and UndefinedBehaviorSanitizer"
echo

# Create security build directory
cd "$TEST_DIR"
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating security build directory..."
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Configure with security sanitizers
echo "Configuring with security sanitizers..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_ASAN=ON \
    -DENABLE_UBSAN=ON

# Build security tests
echo "Building security tests..."
make -j$(nproc)

echo
echo "=== Running Security Tests with Sanitizers ==="

# Run security tests
if make test_security 2>&1; then
    echo "✅ Security tests: PASSED"
    echo "🛡️  Memory safety validation: SUCCESSFUL"
    exit 0
else
    echo "❌ Security tests: FAILED"
    echo "⚠️  Potential memory safety issues detected"
    exit 1
fi
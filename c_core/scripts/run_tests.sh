#!/bin/bash
set -e # Exit if anything fails

echo "⚙️  Building the Factory..."
# 1. Get the directory where THIS script is actually located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# 2. Jump to the parent directory (c_core) so CMake finds the Lists file
cd "$SCRIPT_DIR/.."
cmake -S . -B build
cmake --build build

echo "🧪 Running Quality Control (CTest)..."
echo "-----------------------------------"
cd build
ctest --output-on-failure
echo "-----------------------------------"
echo "✅ All tests passed. The codebase is clean."
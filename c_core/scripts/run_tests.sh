#!/bin/bash
set -e # Exit if anything fails

echo "⚙️  Building the Factory..."
cmake -S . -B build
cmake --build build

echo "🧪 Running Quality Control (CTest)..."
echo "-----------------------------------"
cd build
ctest --output-on-failure
echo "-----------------------------------"
echo "✅ All tests passed. The codebase is clean."
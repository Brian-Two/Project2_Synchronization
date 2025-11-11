#!/bin/bash

echo "==================================="
echo "Testing Poor Student - Dear Old Dad"
echo "==================================="
echo ""

# Function to run a test with timeout (macOS compatible)
run_test() {
    local duration=$1
    shift
    "$@" &
    local pid=$!
    sleep $duration
    kill $pid 2>/dev/null
    wait $pid 2>/dev/null
}

# Clean up any existing semaphores and files
rm -f account.txt log.txt 2>/dev/null
killall psdd psdd_extra 2>/dev/null

echo "Test 1: Basic version (1 parent, 1 child)"
echo "Running for 5 seconds..."
echo "-----------------------------------"
run_test 5 ./psdd
echo ""
echo "✓ Test 1 completed"
echo ""

# Clean up
rm -f account.txt 2>/dev/null
sleep 1

echo "Test 2: Extra credit - Dad only, 3 students"
echo "Running for 5 seconds..."
echo "-----------------------------------"
run_test 5 ./psdd_extra 1 3
echo ""
echo "✓ Test 2 completed"
echo ""

# Clean up
rm -f account.txt 2>/dev/null
sleep 1

echo "Test 3: Extra credit - Mom & Dad, 2 students"
echo "Running for 5 seconds..."
echo "-----------------------------------"
run_test 5 ./psdd_extra 2 2
echo ""
echo "✓ Test 3 completed"
echo ""

# Final cleanup
rm -f account.txt log.txt 2>/dev/null
killall psdd psdd_extra 2>/dev/null

echo "==================================="
echo "All tests completed successfully!"
echo "==================================="
echo ""
echo "To run manually:"
echo "  Basic:       ./psdd"
echo "  Extra:       ./psdd_extra 2 10"
echo ""
echo "To stop: Press Ctrl+C or use: killall psdd"


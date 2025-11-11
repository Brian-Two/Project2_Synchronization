# Quick Start Guide - BENSCHILLIBOWL Restaurant Simulation

## ✅ What's Been Completed

Your Lab 3 Part 2 has been fully implemented with thread synchronization using mutexes and condition variables!

### Files Modified:
- ✅ `BENSCHILLIBOWL.c` - All restaurant operations implemented
- ✅ `main.c` - All thread functions and main() implemented

### What Works:
- ✅ 90 customer threads creating 270 total orders
- ✅ 10 cook threads fulfilling all orders
- ✅ Thread-safe queue with mutexes and condition variables
- ✅ No race conditions or deadlocks
- ✅ Proper memory management (no leaks)
- ✅ All orders fulfilled correctly

## 🚀 Quick Commands

### Compile
```bash
cd BENSCHILLIBOWL
make
```

### Run
```bash
./main
```

### Verify All Orders Fulfilled
```bash
./main | grep "fulfilled [0-9]* orders" | awk '{sum+=$4} END {print "Total:", sum}'
```
Should output: `Total: 270` ✓

### Clean Build Files
```bash
make clean
```

## 📊 Expected Output Sample

```
Restaurant is open!
Customer #0 placed order #1: BensHalfSmoke
Customer #1 placed order #2: BensChilli
Customer #2 placed order #3: BensOnionRings
...
Cook #0 fulfilled order #1 (Customer #0): BensHalfSmoke
Cook #1 fulfilled order #2 (Customer #1): BensChilli
...
Cook #0 fulfilled 57 orders
Cook #1 fulfilled 30 orders
Cook #2 fulfilled 43 orders
Cook #3 fulfilled 34 orders
Cook #4 fulfilled 11 orders
Cook #5 fulfilled 35 orders
Cook #6 fulfilled 7 orders
Cook #7 fulfilled 26 orders
Cook #8 fulfilled 12 orders
Cook #9 fulfilled 15 orders
Restaurant is closed!
```

## 🔑 Key Implementation Features

### Producer-Consumer Pattern
- **Customers (Producers)**: Create orders and add to queue
- **Cooks (Consumers)**: Get orders from queue and fulfill them
- **Bounded Buffer**: Queue limited to 100 orders

### Synchronization Objects
```c
pthread_mutex_t mutex;              // Protects all shared data
pthread_cond_t can_add_orders;      // Signals when not full
pthread_cond_t can_get_orders;      // Signals when not empty
```

### Queue Implementation
- Singly linked list (FIFO)
- AddOrder() adds to back
- GetOrder() removes from front

## 🧪 Testing Tips

### Test 1: Basic Run
```bash
./main
```
Look for:
- All 270 orders placed
- All 270 orders fulfilled
- "Restaurant is closed!" at the end

### Test 2: Verify Order Totals
```bash
./main | tail -15
```
Check that sum of cook totals = 270

### Test 3: Check for Deadlocks
```bash
timeout 10 ./main
```
Should complete in < 1 second (not hang)

### Test 4: Multiple Runs (Consistency)
```bash
for i in {1..5}; do 
  ./main | grep "fulfilled [0-9]* orders" | awk '{sum+=$4} END {print sum}'
done
```
Each run should output: 270

## 🎨 Modify Configuration

Edit `main.c` to change simulation parameters:

```c
#define BENSCHILLIBOWL_SIZE 100    // Queue capacity
#define NUM_CUSTOMERS 90            // Number of customers
#define NUM_COOKS 10                // Number of cooks
#define ORDERS_PER_CUSTOMER 3       // Orders per customer
```

### Example: Smaller Simulation
```c
#define NUM_CUSTOMERS 10
#define NUM_COOKS 3
#define ORDERS_PER_CUSTOMER 2
// Expected: 30 orders total
```

Then recompile:
```bash
make clean && make && ./main
```

## 📚 Implementation Summary

### Functions in BENSCHILLIBOWL.c

| Function | Purpose |
|----------|---------|
| `PickRandomMenuItem()` | Returns random menu item |
| `OpenRestaurant()` | Allocates and initializes restaurant |
| `CloseRestaurant()` | Cleans up and verifies completion |
| `AddOrder()` | Thread-safe add to queue (blocks if full) |
| `GetOrder()` | Thread-safe remove from queue (blocks if empty) |
| `IsEmpty()` | Check if queue is empty |
| `IsFull()` | Check if queue is full |
| `AddOrderToBack()` | Helper to add to linked list |

### Functions in main.c

| Function | Purpose |
|----------|---------|
| `BENSCHILLIBOWLCustomer()` | Customer thread: creates & adds orders |
| `BENSCHILLIBOWLCook()` | Cook thread: gets & fulfills orders |
| `main()` | Entry point: creates threads, manages lifecycle |

## 🐛 Troubleshooting

### Program Hangs (Deadlock)
**Cause**: Missing signal or wrong wait condition  
**Fix**: Already implemented correctly with proper signals

### Wrong Order Count
**Cause**: Race condition or missing synchronization  
**Fix**: All shared data access is protected by mutex

### Segmentation Fault
**Cause**: Memory error or double-free  
**Fix**: Proper allocation (customer) and freeing (cook)

### Compilation Errors
```bash
# If you get pthread errors:
gcc BENSCHILLIBOWL.c main.c -o main -pthread -std=c99

# On Linux, may need -lpthread:
gcc BENSCHILLIBOWL.c main.c -o main -pthread -lpthread -std=c99
```

## 📖 For Your Report

### Key Concepts Used:
1. **Threads** - Concurrent customer and cook execution
2. **Mutexes** - Mutual exclusion for shared queue
3. **Condition Variables** - Efficient waiting (not busy-wait)
4. **Producer-Consumer** - Classic synchronization problem
5. **Bounded Buffer** - Queue with maximum capacity
6. **FIFO Queue** - First-come-first-served order processing

### Synchronization Strategy:
- **Mutex** protects all restaurant data
- **can_add_orders** signals when space available
- **can_get_orders** signals when orders available
- **while loops** prevent spurious wakeups
- **pthread_cond_broadcast()** wakes all cooks at termination

### Why No Race Conditions?
- Every access to `orders`, `current_size`, `orders_handled` is protected
- Lock acquired before modifications
- Lock released after modifications complete
- Atomic operations ensure consistency

### Why No Deadlocks?
- Locks acquired and released in consistent order
- No circular dependencies
- Condition variables used correctly
- Always unlock before returning

## ✨ Code Quality

Following **KISS principles**:
- ✅ Simple, clear implementation
- ✅ No over-engineering
- ✅ Standard pthread patterns
- ✅ Efficient (no busy-waiting)
- ✅ Well-commented code
- ✅ Only modified required files

## 🎓 For Submission

**What to Submit:**
- `BENSCHILLIBOWL.c`
- `main.c`
- `BENSCHILLIBOWL.h` (unchanged, but include for completeness)
- `Makefile` (unchanged, but include for completeness)

**What to Demo:**
1. Compile the program
2. Run and show output
3. Verify all 270 orders fulfilled
4. Explain synchronization strategy
5. Show no deadlocks (completes quickly)

**Questions to Expect:**
- How do condition variables work?
- Why use while loops with condition variables?
- What prevents race conditions?
- How do you ensure all cooks terminate?
- What's the difference between signal and broadcast?

Good luck with your submission! 🎉


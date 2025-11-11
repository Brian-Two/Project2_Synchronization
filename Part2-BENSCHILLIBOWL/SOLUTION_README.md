# BENSCHILLIBOWL Restaurant Simulation - Solution

## Overview
This solution implements a multi-threaded restaurant simulation using **pthreads**, **mutexes**, and **condition variables**. It's a classic **Producer-Consumer** problem where:
- **Customers (Producers)** create orders and add them to a queue
- **Cooks (Consumers)** take orders from the queue and fulfill them
- **Synchronization** prevents race conditions using monitors

## Implementation Details

### Data Structures

**Order Queue**: Implemented as a **singly linked list** (FIFO)
- `AddOrder()` adds to the back of the queue
- `GetOrder()` removes from the front of the queue

**Restaurant Struct** (`BENSCHILLIBOWL`):
```c
- Order* orders           // Head of the linked list queue
- int current_size        // Number of orders currently in queue
- int max_size            // Maximum queue capacity (100)
- int next_order_number   // Counter for order numbers
- int orders_handled      // Total orders fulfilled by cooks
- int expected_num_orders // Total orders expected (270 = 90 × 3)
- pthread_mutex_t mutex   // Protects all restaurant data
- pthread_cond_t can_add_orders   // Signals when queue is not full
- pthread_cond_t can_get_orders   // Signals when queue is not empty
```

### Synchronization Strategy

#### Producer (Customer) - AddOrder()
```
1. Lock mutex
2. Wait while queue is FULL (using condition variable)
3. Add order to back of queue
4. Increment current_size
5. Signal can_get_orders (wake up waiting cooks)
6. Unlock mutex
```

#### Consumer (Cook) - GetOrder()
```
1. Lock mutex
2. Wait while queue is EMPTY AND not all orders handled
3. If all orders handled, return NULL and wake up other cooks
4. Remove order from front of queue
5. Decrement current_size
6. Increment orders_handled
7. Signal can_add_orders (wake up waiting customers)
8. Unlock mutex
9. Return order
```

### Key Features

✅ **Thread-Safe Queue Operations**
- All queue modifications protected by mutex
- Condition variables prevent busy waiting

✅ **Bounded Buffer**
- Queue limited to 100 orders maximum
- Customers wait when queue is full

✅ **Graceful Termination**
- Cooks exit when all expected orders are handled
- Uses `pthread_cond_broadcast()` to wake all waiting cooks

✅ **No Race Conditions**
- Proper use of mutexes ensures mutual exclusion
- Condition variables ensure correct timing

✅ **Memory Management**
- Orders allocated by customers, freed by cooks
- Restaurant resources properly cleaned up

## Files Modified

### BENSCHILLIBOWL.c
**Functions Implemented:**
- `PickRandomMenuItem()` - Returns random menu item
- `OpenRestaurant()` - Allocates and initializes restaurant
- `CloseRestaurant()` - Verifies completion and frees resources
- `AddOrder()` - Adds order to queue (with synchronization)
- `GetOrder()` - Removes order from queue (with synchronization)
- `IsEmpty()` - Checks if queue is empty
- `IsFull()` - Checks if queue is full
- `AddOrderToBack()` - Helper to add to linked list tail

### main.c
**Functions Implemented:**
- `BENSCHILLIBOWLCustomer()` - Customer thread function
  - Creates 3 orders per customer
  - Allocates memory for each order
  - Picks random menu items
  - Adds orders to restaurant
  
- `BENSCHILLIBOWLCook()` - Cook thread function
  - Gets orders in a loop
  - Fulfills and frees each order
  - Exits when no more orders
  - Reports total orders fulfilled
  
- `main()` - Program entry point
  - Opens restaurant
  - Creates 90 customer threads
  - Creates 10 cook threads
  - Joins all threads
  - Closes restaurant

## Configuration

```c
#define BENSCHILLIBOWL_SIZE 100    // Max queue size
#define NUM_CUSTOMERS 90            // Number of customer threads
#define NUM_COOKS 10                // Number of cook threads
#define ORDERS_PER_CUSTOMER 3       // Orders each customer makes
#define EXPECTED_NUM_ORDERS 270     // Total: 90 × 3
```

Feel free to modify these values to test different scenarios!

## How to Compile and Run

### Compile
```bash
make
```

Or manually:
```bash
gcc -c -o BENSCHILLIBOWL.o BENSCHILLIBOWL.c -I. -pthread -std=c99
gcc -c -o main.o main.c -I. -pthread -std=c99
gcc -o main BENSCHILLIBOWL.o main.o -I. -pthread -std=c99
```

### Run
```bash
./main
```

### Clean
```bash
make clean
```

## Expected Output

```
Restaurant is open!
Customer #0 placed order #1: BensHalfSmoke
Customer #1 placed order #2: BensChilli
...
Cook #0 fulfilled order #1 (Customer #0): BensHalfSmoke
Cook #1 fulfilled order #2 (Customer #1): BensChilli
...
Cook #0 fulfilled 57 orders
Cook #1 fulfilled 30 orders
...
Cook #9 fulfilled 15 orders
Restaurant is closed!
```

### Verify Correctness
Check that total orders fulfilled = 270:
```bash
./main | grep "fulfilled [0-9]* orders" | awk '{sum+=$4} END {print "Total:", sum}'
```

Should output: `Total: 270`

## Testing Different Scenarios

### Test with Small Numbers
```c
#define NUM_CUSTOMERS 5
#define NUM_COOKS 2
#define ORDERS_PER_CUSTOMER 2
// Expected: 10 orders total
```

### Test with Large Queue
```c
#define BENSCHILLIBOWL_SIZE 500
```

### Test Queue Blocking (Small Buffer)
```c
#define BENSCHILLIBOWL_SIZE 5  // Very small queue
// Customers will frequently wait for space
```

## Common Issues and Solutions

### Problem: Deadlock (program hangs)
**Solution:** Make sure condition variables are properly signaled
- `AddOrder()` signals `can_get_orders`
- `GetOrder()` signals `can_add_orders`

### Problem: Not all orders fulfilled
**Solution:** Check the termination condition in `GetOrder()`
- Must use `pthread_cond_broadcast()` to wake all cooks
- Cooks must check `orders_handled >= expected_num_orders`

### Problem: Segmentation fault
**Solution:** Check memory management
- Customers allocate orders
- Cooks free orders
- No double-free issues

### Problem: Race conditions (wrong totals)
**Solution:** All shared data access must be in critical sections
- Lock mutex before accessing `orders`, `current_size`, etc.
- Unlock after modifications complete

## Key Concepts Demonstrated

1. **Producer-Consumer Pattern**
   - Multiple producers (customers) and consumers (cooks)
   - Bounded buffer (queue with max size)

2. **Thread Synchronization**
   - Mutexes for mutual exclusion
   - Condition variables for efficient waiting

3. **Monitor Pattern**
   - High-level synchronization construct
   - Mutex + condition variables = monitor

4. **FIFO Queue**
   - First In, First Out order processing
   - Implemented with linked list

5. **Thread Creation and Joining**
   - Dynamic thread creation
   - Proper cleanup with pthread_join

## Grading Checklist

✅ OpenRestaurant() - Allocates and initializes restaurant  
✅ CloseRestaurant() - Verifies completion and frees resources  
✅ PickRandomMenuItem() - Returns random menu item  
✅ AddOrder() - Thread-safe order addition with blocking  
✅ GetOrder() - Thread-safe order removal with blocking  
✅ BENSCHILLIBOWLCustomer() - Customer creates and adds orders  
✅ BENSCHILLIBOWLCook() - Cook gets and fulfills orders  
✅ main() - Creates threads and manages restaurant lifecycle  
✅ No race conditions  
✅ No deadlocks  
✅ Proper memory management  
✅ All orders fulfilled correctly  

## Additional Notes

**Why use condition variables instead of busy-waiting?**
- More efficient (CPU doesn't spin)
- Threads sleep until signaled
- Better performance with many threads

**Why use pthread_cond_broadcast() in GetOrder()?**
- When all orders are done, ALL cooks need to wake up and exit
- pthread_cond_signal() only wakes one thread
- Ensures all cooks terminate properly

**Why check the condition in a while loop?**
```c
while (IsFull(bcb)) {
    pthread_cond_wait(...);
}
```
- Prevents spurious wakeups
- Handles race conditions between signal and wakeup
- Standard practice for condition variables

Good luck with your lab! 🎉


# Poor Student - Dear Old Dad Solution

## Overview
This solution implements the classic IPC synchronization problem using semaphores to prevent race conditions when accessing a shared BankAccount.

## Files Created

1. **psdd.c** - Basic implementation (1 parent + 1 child)
2. **psdd_extra.c** - Extra credit implementation (supports Mom & Dad + N children)

## How to Compile

```bash
# Compile basic version
make psdd

# Compile extra credit version
make psdd_extra
```

## How to Run

### Basic Version (psdd)
```bash
./psdd
```
This runs Dear Old Dad (parent) and one Poor Student (child) indefinitely.

### Extra Credit Version (psdd_extra)
```bash
# Syntax: ./psdd_extra <numParents> <numChildren>

# Run with just Dad and 1 student:
./psdd_extra 1 1

# Run with just Dad and 3 students:
./psdd_extra 1 3

# Run with both Mom and Dad, and 10 students:
./psdd_extra 2 10
```

## How to Stop the Program

Since both programs run indefinitely, you need to kill them manually:

1. Open a new terminal
2. Find the process ID:
   ```bash
   ps -eaf | grep psdd
   ```
3. Kill the parent process:
   ```bash
   kill <parent_pid>
   ```

## Key Features

### Synchronization
- Uses **semaphores** (`sem_wait` and `sem_post`) to ensure mutual exclusion
- Prevents race conditions when multiple processes access the shared BankAccount
- Only one process can be in the critical section at a time

### Shared Memory
- Uses memory-mapped files (`mmap`) to share the BankAccount between processes
- The BankAccount is accessible to all parent and child processes

### Process Behavior

#### Dear Old Dad (Parent)
- Sleeps 0-5 seconds randomly
- On even random numbers: checks if balance < 100, then attempts to deposit $0-100
- On odd random numbers: just checks the balance
- May decide not to give money (odd deposit random)

#### Lovable Mom (Parent - Extra Credit)
- Sleeps 0-10 seconds randomly
- If balance ≤ 100, always deposits $125-225
- More generous than Dad!

#### Poor Student (Child)
- Sleeps 0-5 seconds randomly
- On even random numbers: attempts to withdraw $0-50
- On odd random numbers: just checks the balance
- Can only withdraw if sufficient funds are available

## Implementation Notes

### KISS Principle
- Simple, clean implementation following the example.c pattern
- No over-engineering - just the essentials
- Clear separation of concerns with dedicated functions

### Race Condition Prevention
- All reads and writes to BankAccount are protected by semaphore
- Local variables used to minimize time in critical section
- Proper sem_wait/sem_post pairing

### Random Number Generation
- Each process seeds random generator with `time(NULL) + getpid()`
- Ensures different random sequences for each process
- Realistic simulation of asynchronous behavior

## Testing

Compare your output with the reference implementation:
```bash
./ref_psdd
```

Look for:
- No negative balances (unless there's a bug)
- Proper mutual exclusion (no interleaved critical section messages)
- Correct deposit/withdrawal logic
- Accurate balance tracking

## Common Issues and Solutions

### Semaphore Already Exists
If you see "semaphore initialization" errors:
```bash
# Clean up old semaphores
rm /dev/shm/sem.banksemaphore
```

### Zombie Processes
If processes don't terminate cleanly:
```bash
# Find and kill all psdd processes
killall psdd
killall psdd_extra
```

### File Cleanup
Remove temporary files:
```bash
rm account.txt log.txt
```

## Extra Credit Grading

The extra credit implementation (`psdd_extra.c`) includes:
- ✅ Lovable Mom parent process with custom rules
- ✅ Support for N children (command line argument)
- ✅ Proper semaphore synchronization for all processes
- ✅ Unique student numbering for multiple children
- ✅ Different sleep patterns for Mom (0-10s) vs Dad (0-5s)

Worth **4 points** extra credit!


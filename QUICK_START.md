# Quick Start Guide - Lab 3 Part 1 Solution

## ✅ What's Been Completed

Your lab has been fully implemented with both the **basic** and **extra credit** versions!

### Files Created:
- `psdd.c` - Basic version (Dear Old Dad + 1 Poor Student)
- `psdd_extra.c` - Extra credit (Mom & Dad + N students)
- `test.sh` - Automated test script
- `SOLUTION_README.md` - Comprehensive documentation

## 🚀 Quick Commands

### Build Everything
```bash
make psdd
make psdd_extra
```

### Run Basic Version
```bash
./psdd
```
- Runs indefinitely with Dear Old Dad (parent) and one Poor Student (child)
- Press `Ctrl+C` to stop

### Run Extra Credit Version
```bash
# Syntax: ./psdd_extra <numParents> <numChildren>

# Examples:
./psdd_extra 1 1    # Just Dad + 1 student (basic problem)
./psdd_extra 1 3    # Just Dad + 3 students
./psdd_extra 2 5    # Mom & Dad + 5 students
./psdd_extra 2 10   # Mom & Dad + 10 students (full extra credit)
```

### Run Tests
```bash
./test.sh
```
Runs all three scenarios for 5 seconds each to verify correct behavior.

## 🔑 Key Features Implemented

### ✅ Synchronization
- Semaphores protect the shared BankAccount
- No race conditions - mutual exclusion enforced
- Clean critical sections with `sem_wait()` and `sem_post()`

### ✅ Process Behaviors

**Dear Old Dad:**
- Sleeps 0-5 seconds
- Deposits when balance < $100 (maybe $0-$100)
- Can decide not to give money
- Thinks student has enough when balance ≥ $100

**Lovable Mom (Extra Credit):**
- Sleeps 0-10 seconds  
- Always deposits $125-$225 when balance ≤ $100
- More generous than Dad!

**Poor Student(s):**
- Sleep 0-5 seconds
- Try to withdraw $0-$50 randomly
- Can only withdraw if funds available
- Each student numbered in multi-student scenarios

### ✅ Output Format
All printf statements match the exact format specified in the lab requirements.

## 📊 Expected Output Example

```
Dear Old Dad: Attempting to Check Balance
Dear Old Dad: Last Checking Balance = $0
Poor Student: Attempting to Check Balance
Poor Student needs $25
Poor Student: Not Enough Cash ($0)
Dear old Dad: Attempting to Check Balance
Dear old Dad: Deposits $75 / Balance = $75
Poor Student: Attempting to Check Balance
Poor Student needs $30
Poor Student: Withdraws $30 / Balance = $45
...
```

## 🛠️ Troubleshooting

### Kill Running Programs
```bash
killall psdd
killall psdd_extra
```

### Clean Up Files
```bash
rm -f account.txt log.txt
```

### If Semaphore Issues
```bash
# On Linux:
rm -f /dev/shm/sem.banksemaphore

# On macOS:
# Semaphores are cleaned up automatically
```

## 📝 Testing Against Reference

Compare with the provided reference implementation:
```bash
./ref_psdd
```

Look for similar patterns:
- Random timing between operations
- Correct deposit/withdrawal logic
- Balance never goes negative
- Proper synchronization (no garbled output)

## 🎓 Extra Credit Verification

To get full extra credit points, demonstrate:

```bash
# Run with 2 parents and 10 students
./psdd_extra 2 10
```

Let it run for ~30 seconds to show:
- ✅ Both Mom and Dad depositing
- ✅ Multiple students (numbered 1-10) withdrawing
- ✅ Mom's longer sleep (0-10s) vs Dad's (0-5s)
- ✅ Mom's larger deposits ($125-$225)
- ✅ No race conditions with many processes

## 💡 Code Quality Notes

The solution follows **KISS principles**:
- Simple, clean implementation
- No over-engineering
- Based on the provided `example.c` pattern
- Efficient critical sections (minimal time holding semaphore)
- Clear separation of concerns

## 📚 For Your Report

Key points to mention:
1. **Semaphores ensure mutual exclusion** - only one process accesses BankAccount at a time
2. **Memory-mapped files** create shared memory between parent and child processes
3. **Critical sections** are kept minimal for efficiency
4. **Random behavior** simulates asynchronous real-world scenarios
5. **Multiple processes** (extra credit) demonstrate scalability of the solution

Good luck with your lab submission! 🎉


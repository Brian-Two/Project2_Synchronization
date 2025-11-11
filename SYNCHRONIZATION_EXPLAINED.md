# Synchronization Concepts Explained

## The Race Condition Problem

Without synchronization, this could happen:

```
Time  | Dear Old Dad               | Poor Student
------|----------------------------|---------------------------
T1    | Read BankAccount = $100    |
T2    |                            | Read BankAccount = $100
T3    | Add $50 → localBalance=$150|
T4    |                            | Subtract $30 → localBalance=$70
T5    | Write $150 to BankAccount  |
T6    |                            | Write $70 to BankAccount
```

**Result:** Final balance is $70, but it should be $120! Dad's $50 deposit was lost.

This is called a **race condition** - the outcome depends on the timing of events.

## The Solution: Semaphores

A **semaphore** is like a token that only one process can hold at a time.

### Semaphore Operations

```c
sem_wait(mutex);   // DOWN operation - acquire the lock
// CRITICAL SECTION - only one process can be here
sem_post(mutex);   // UP operation - release the lock
```

### How It Works

```
Time  | Dear Old Dad                    | Poor Student
------|--------------------------------|--------------------------------
T1    | sem_wait(mutex) ✓              |
T2    | Read BankAccount = $100        |
T3    |                                | sem_wait(mutex) ⏸️ [BLOCKED]
T4    | Add $50 → localBalance=$150    |
T5    | Write $150 to BankAccount      |
T6    | sem_post(mutex) ✓              |
T7    |                                | sem_wait(mutex) ✓ [UNBLOCKED]
T8    |                                | Read BankAccount = $150
T9    |                                | Subtract $30 → localBalance=$120
T10   |                                | Write $120 to BankAccount
T11   |                                | sem_post(mutex) ✓
```

**Result:** Final balance is $120 ✅ Correct!

## Code Structure

### Basic Pattern (from example.c)

```c
// 1. Create shared memory
fd = open("account.txt", O_RDWR | O_CREAT, S_IRWXU);
BankAccount = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

// 2. Create semaphore (initialized to 1 = available)
mutex = sem_open("banksemaphore", O_CREAT, 0644, 1);

// 3. In each process, protect critical sections
sem_wait(mutex);        // Wait if someone else is inside
// ... access BankAccount ...
sem_post(mutex);        // Release for others
```

## Our Implementation

### Dear Old Dad's Deposit Logic

```c
sem_wait(mutex);                    // 🔒 Lock
localBalance = *BankAccount;        // Read shared variable
amount = rand() % 101;              // Generate random amount
localBalance += amount;             // Modify local copy
*BankAccount = localBalance;        // Write back to shared variable
printf("Deposits $%d / Balance = $%d\n", amount, localBalance);
sem_post(mutex);                    // 🔓 Unlock
```

### Poor Student's Withdraw Logic

```c
sem_wait(mutex);                    // 🔒 Lock
localBalance = *BankAccount;        // Read shared variable
need = rand() % 51;                 // How much student needs

if (need <= localBalance) {
    localBalance -= need;           // Modify local copy
    *BankAccount = localBalance;    // Write back
    printf("Withdraws $%d / Balance = $%d\n", need, localBalance);
} else {
    printf("Not Enough Cash ($%d)\n", localBalance);
}
sem_post(mutex);                    // 🔓 Unlock
```

## Why Use Local Variables?

Notice we copy `*BankAccount` to `localBalance`:

```c
localBalance = *BankAccount;    // Copy from shared to local
// ... do calculations on localBalance ...
*BankAccount = localBalance;    // Copy from local back to shared
```

**Reasons:**
1. **Minimize critical section** - do calculations outside the lock when possible
2. **Clear code** - easier to see what's being modified
3. **Atomic update** - the final write happens in one operation

## Multiple Processes (Extra Credit)

With Mom, Dad, and multiple students, the semaphore ensures:

```
Process    | Action                  | Semaphore Value
-----------|-------------------------|----------------
Mom        | sem_wait(mutex)         | 1 → 0 (acquired)
Dad        | sem_wait(mutex)         | 0 → wait...
Student 1  | sem_wait(mutex)         | 0 → wait...
Student 2  | sem_wait(mutex)         | 0 → wait...
Mom        | ... doing work ...      | 0 (still held)
Mom        | sem_post(mutex)         | 0 → 1 (released)
Dad        | sem_wait(mutex) ✓       | 1 → 0 (acquired)
...
```

Only ONE process can be in the critical section at any time, regardless of how many total processes exist.

## Common Mistakes Avoided

### ❌ Wrong: Reading outside critical section

```c
localBalance = *BankAccount;    // Race condition!
sem_wait(mutex);
localBalance += amount;
*BankAccount = localBalance;
sem_post(mutex);
```

**Problem:** Another process could modify BankAccount between read and lock.

### ✅ Correct: Read inside critical section

```c
sem_wait(mutex);
localBalance = *BankAccount;    // Safe!
localBalance += amount;
*BankAccount = localBalance;
sem_post(mutex);
```

### ❌ Wrong: Forgetting to unlock

```c
sem_wait(mutex);
if (error) {
    return;    // Oops! Never called sem_post()
}
sem_post(mutex);
```

**Problem:** Semaphore stays locked forever (deadlock).

### ✅ Correct: Always unlock

```c
sem_wait(mutex);
if (error) {
    sem_post(mutex);    // Unlock before returning
    return;
}
sem_post(mutex);
```

## Testing for Race Conditions

Run with many processes to stress-test:

```bash
./psdd_extra 2 10    # 2 parents + 10 children = 12 processes
```

Watch for:
- ✅ No negative balances
- ✅ No garbled/overlapping output
- ✅ Deposits and withdrawals add up correctly
- ✅ No process gets starved (all make progress)

## Key Takeaways

1. **Semaphores prevent race conditions** by enforcing mutual exclusion
2. **Critical sections** should be kept as small as possible
3. **Shared memory** allows processes to communicate
4. **Always pair** `sem_wait()` with `sem_post()`
5. **Test with many processes** to reveal subtle bugs

## Further Reading

- OS Textbook Chapter 5 (Process Synchronization)
- Classic problems: Producer-Consumer, Readers-Writers, Dining Philosophers
- Advanced: Monitors, Condition Variables, Message Passing

---

*This solution demonstrates fundamental OS concepts that are used in:*
- Database transaction management
- Multi-threaded web servers
- Operating system kernel design
- Distributed systems
- Any concurrent programming scenario


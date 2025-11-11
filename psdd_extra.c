#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/wait.h>

void DearOldDad(int *BankAccount, sem_t *mutex);
void LovableMom(int *BankAccount, sem_t *mutex);
void PoorStudent(int *BankAccount, sem_t *mutex, int studentNum);

int main(int argc, char **argv)
{
    int numParents, numChildren;
    int fd, zero = 0;
    int *BankAccount;
    sem_t *mutex;
    
    if (argc != 3) {
        printf("Usage: %s <numParents> <numChildren>\n", argv[0]);
        printf("  numParents: 1 (Dad only) or 2 (Dad and Mom)\n");
        printf("  numChildren: number of students (1-N)\n");
        exit(1);
    }
    
    numParents = atoi(argv[1]);
    numChildren = atoi(argv[2]);
    
    if (numParents < 1 || numParents > 2) {
        printf("Error: numParents must be 1 or 2\n");
        exit(1);
    }
    
    if (numChildren < 1) {
        printf("Error: numChildren must be at least 1\n");
        exit(1);
    }
    
    // Seed random number generator with process ID for variation
    srand(time(NULL) + getpid());
    
    // Create a file and map it into memory for shared BankAccount
    fd = open("account.txt", O_RDWR | O_CREAT, S_IRWXU);
    write(fd, &zero, sizeof(int));
    BankAccount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    // Initialize BankAccount to 0
    *BankAccount = 0;
    
    // Create semaphore for mutual exclusion
    sem_unlink("banksemaphore");  // Remove any existing semaphore
    if ((mutex = sem_open("banksemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("semaphore initialization");
        exit(1);
    }
    
    // Create parent processes
    if (fork() == 0) {
        // First parent - Dear Old Dad
        srand(time(NULL) + getpid());
        DearOldDad(BankAccount, mutex);
        exit(0);
    }
    
    if (numParents == 2) {
        if (fork() == 0) {
            // Second parent - Lovable Mom
            srand(time(NULL) + getpid());
            LovableMom(BankAccount, mutex);
            exit(0);
        }
    }
    
    // Create children processes
    for (int i = 0; i < numChildren; i++) {
        if (fork() == 0) {
            // Child process - Poor Student
            srand(time(NULL) + getpid() + i);
            PoorStudent(BankAccount, mutex, i + 1);
            exit(0);
        }
    }
    
    // Parent waits for all children (will run indefinitely until killed)
    while (1) {
        wait(NULL);
    }
    
    exit(0);
}

void DearOldDad(int *BankAccount, sem_t *mutex)
{
    int localBalance;
    int amount;
    
    while (1) {
        sleep(rand() % 6);  // Sleep 0-5 seconds
        
        printf("Dear Old Dad: Attempting to Check Balance\n");
        
        int randomNum = rand();
        
        if (randomNum % 2 == 0) {
            // Even number - check if need to deposit
            sem_wait(mutex);
            localBalance = *BankAccount;
            sem_post(mutex);
            
            if (localBalance < 100) {
                // Try to deposit
                int depositRandom = rand();
                
                if (depositRandom % 2 == 0) {
                    // Even - deposit money
                    sem_wait(mutex);  // Enter critical section
                    
                    localBalance = *BankAccount;
                    amount = rand() % 101;  // Random amount 0-100
                    
                    if (amount > 0) {
                        localBalance += amount;
                        *BankAccount = localBalance;
                        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
                    }
                    
                    sem_post(mutex);  // Leave critical section
                } else {
                    // Odd - no money to give
                    printf("Dear old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
            }
        } else {
            // Odd number - just check balance
            sem_wait(mutex);
            localBalance = *BankAccount;
            sem_post(mutex);
            printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
        }
    }
}

void LovableMom(int *BankAccount, sem_t *mutex)
{
    int localBalance;
    int amount;
    
    while (1) {
        sleep(rand() % 11);  // Sleep 0-10 seconds
        
        printf("Loveable Mom: Attempting to Check Balance\n");
        
        sem_wait(mutex);
        localBalance = *BankAccount;
        sem_post(mutex);
        
        if (localBalance <= 100) {
            // Always deposit when balance is low
            sem_wait(mutex);  // Enter critical section
            
            localBalance = *BankAccount;
            amount = (rand() % 101) + 125;  // Random amount 125-225
            
            localBalance += amount;
            *BankAccount = localBalance;
            printf("Lovable Mom: Deposits $%d / Balance = $%d\n", amount, localBalance);
            
            sem_post(mutex);  // Leave critical section
        }
    }
}

void PoorStudent(int *BankAccount, sem_t *mutex, int studentNum)
{
    int localBalance;
    int need;
    
    while (1) {
        sleep(rand() % 6);  // Sleep 0-5 seconds
        
        printf("Poor Student %d: Attempting to Check Balance\n", studentNum);
        
        int randomNum = rand();
        
        if (randomNum % 2 == 0) {
            // Even number - attempt to withdraw
            sem_wait(mutex);  // Enter critical section
            
            localBalance = *BankAccount;
            need = rand() % 51;  // Random need 0-50
            
            printf("Poor Student %d needs $%d\n", studentNum, need);
            
            if (need <= localBalance) {
                localBalance -= need;
                *BankAccount = localBalance;
                printf("Poor Student %d: Withdraws $%d / Balance = $%d\n", studentNum, need, localBalance);
            } else {
                printf("Poor Student %d: Not Enough Cash ($%d)\n", studentNum, localBalance);
            }
            
            sem_post(mutex);  // Leave critical section
        } else {
            // Odd number - just check balance
            sem_wait(mutex);
            localBalance = *BankAccount;
            sem_post(mutex);
            printf("Poor Student %d: Last Checking Balance = $%d\n", studentNum, localBalance);
        }
    }
}


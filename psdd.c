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

int main(int argc, char **argv)
{
    int fd, zero = 0;
    int *BankAccount;
    sem_t *mutex;
    
    // Seed random number generator
    srand(time(NULL));
    
    // Create a file and map it into memory for shared BankAccount
    fd = open("account.txt", O_RDWR | O_CREAT, S_IRWXU);
    write(fd, &zero, sizeof(int));
    BankAccount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    // Initialize BankAccount to 0
    *BankAccount = 0;
    
    // Create semaphore for mutual exclusion
    if ((mutex = sem_open("banksemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("semaphore initialization");
        exit(1);
    }
    
    if (fork() == 0) {
        /* Child Process - Poor Student */
        int localBalance;
        int need;
        
        while (1) {
            sleep(rand() % 6);  // Sleep 0-5 seconds
            
            printf("Poor Student: Attempting to Check Balance\n");
            
            int randomNum = rand();
            
            if (randomNum % 2 == 0) {
                // Even number - attempt to withdraw
                sem_wait(mutex);  // Enter critical section
                
                localBalance = *BankAccount;
                need = rand() % 51;  // Random need 0-50
                
                printf("Poor Student needs $%d\n", need);
                
                if (need <= localBalance) {
                    localBalance -= need;
                    *BankAccount = localBalance;
                    printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
                } else {
                    printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
                }
                
                sem_post(mutex);  // Leave critical section
            } else {
                // Odd number - just check balance
                sem_wait(mutex);
                localBalance = *BankAccount;
                sem_post(mutex);
                printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
            }
        }
        exit(0);
    }
    
    /* Parent Process - Dear Old Dad */
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
    
    exit(0);
}


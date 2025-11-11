#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    return BENSCHILLIBOWLMenu[rand() % BENSCHILLIBOWLMenuLength];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL* bcb = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
    
    // Initialize restaurant variables
    bcb->orders = NULL;  // Empty queue (linked list)
    bcb->current_size = 0;
    bcb->max_size = max_size;
    bcb->next_order_number = 1;
    bcb->orders_handled = 0;
    bcb->expected_num_orders = expected_num_orders;
    
    // Initialize synchronization objects
    pthread_mutex_init(&(bcb->mutex), NULL);
    pthread_cond_init(&(bcb->can_add_orders), NULL);
    pthread_cond_init(&(bcb->can_get_orders), NULL);
    
    printf("Restaurant is open!\n");
    return bcb;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    // Verify all orders were handled
    if (bcb->orders_handled != bcb->expected_num_orders) {
        fprintf(stderr, "Warning: Expected %d orders, but handled %d\n",
                bcb->expected_num_orders, bcb->orders_handled);
    }
    
    // Destroy synchronization objects
    pthread_mutex_destroy(&(bcb->mutex));
    pthread_cond_destroy(&(bcb->can_add_orders));
    pthread_cond_destroy(&(bcb->can_get_orders));
    
    // Free the restaurant
    free(bcb);
    
    printf("Restaurant is closed!\n");
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&(bcb->mutex));
    
    // Wait while restaurant is full
    while (IsFull(bcb)) {
        pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
    }
    
    // Assign order number
    order->order_number = bcb->next_order_number;
    bcb->next_order_number++;
    
    // Add order to the back of the queue
    AddOrderToBack(&(bcb->orders), order);
    bcb->current_size++;
    
    // Signal that an order is available
    pthread_cond_signal(&(bcb->can_get_orders));
    
    pthread_mutex_unlock(&(bcb->mutex));
    
    return order->order_number;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));
    
    // Wait while restaurant is empty AND we haven't handled all orders
    while (IsEmpty(bcb) && bcb->orders_handled < bcb->expected_num_orders) {
        pthread_cond_wait(&(bcb->can_get_orders), &(bcb->mutex));
    }
    
    // If all orders have been handled, return NULL
    if (bcb->orders_handled >= bcb->expected_num_orders) {
        pthread_cond_broadcast(&(bcb->can_get_orders));  // Wake up other cooks
        pthread_mutex_unlock(&(bcb->mutex));
        return NULL;
    }
    
    // Get order from front of queue
    Order *order = bcb->orders;
    if (order != NULL) {
        bcb->orders = order->next;  // Remove from front
        bcb->current_size--;
        bcb->orders_handled++;
        
        // Signal that space is available
        pthread_cond_signal(&(bcb->can_add_orders));
    }
    
    pthread_mutex_unlock(&(bcb->mutex));
    
    return order;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
  return bcb->current_size == 0;
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  return bcb->current_size >= bcb->max_size;
}

/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
    order->next = NULL;
    
    // If queue is empty, this is the first order
    if (*orders == NULL) {
        *orders = order;
    } else {
        // Traverse to the end of the queue
        Order *current = *orders;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = order;
    }
}


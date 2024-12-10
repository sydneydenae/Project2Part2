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
   
    int randItemIndex = rand() % BENSCHILLIBOWLMenuLength;
    return BENSCHILLIBOWLMenu[randItemIndex];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    
    BENSCHILLIBOWL *restaurant = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
    if (restaurant == NULL){
        printf("Could not allocate memory for the restaurant.");
        return NULL;
    }

    restaurant -> max_size = max_size;
    restaurant -> expected_num_orders = expected_num_orders;
    restaurant -> current_size = 0;
    restaurant -> next_order_number = 1;
    restaurant -> orders_handled = 0;
    restaurant -> orders = NULL;

    // Initialize mutex.
    if(pthread_mutex_init(&restaurant->mutex, NULL) != 0) {
        printf("Error: Failed mutex initialization.");
        free(restaurant);
        return NULL;
    }

    // Initialize condition vars.
    if (pthread_cond_init(&restaurant->can_add_orders, NULL) != 0){
        printf("Condition var 'can_add_orders' initialization failed.");
        pthread_mutex_destroy(&restaurant->mutex);
        free(restaurant);
        return NULL;
    }

    if (pthread_cond_init(&restaurant->can_get_orders, NULL) != 0){
        printf("Condition var 'can_get_orders' initialization failed.");
        pthread_cond_destroy(&restaurant->can_add_orders);
        pthread_mutex_destroy(&restaurant->mutex);
        free(restaurant);
        return NULL;
    }
    // seed random time
      srand(time(NULL));
    printf("Restaurant is open!\n");
    return restaurant;
}

void freeOrders(Order* orders){
    Order* current = orders;
    while(current != NULL){
        Order* next = current -> next;
        free(current);
        current = next;
    }
}

/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */
void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    if(bcb == NULL){
        printf("Restaurant pointer is NULL.");
        return;
    }
    if(bcb -> orders_handled == bcb -> expected_num_orders){
        printf("Restaurant is closed!\n");
    } else {
        printf("Restaurant is not closed!\n");
        return;
    }
    // Free orders.
    freeOrders(bcb -> orders);

    // Destroy the mutex.
    pthread_mutex_destroy(&bcb->mutex);

    // Destroy condition vars.
    pthread_cond_destroy(&bcb -> can_add_orders);
    pthread_cond_destroy(&bcb -> can_get_orders);

    free(bcb);
    printf("Restaurant resources deallocated.");
    return;
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    if(bcb == NULL || order == NULL){
        return -1;
    }

    pthread_mutex_lock(&(bcb->mutex));
    while(bcb -> current_size >= bcb -> max_size){
        pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
    }
    int next_order_number = bcb -> current_size + 1;
    order -> order_number = next_order_number;
    AddOrderToBack(&(bcb -> orders), order);
    bcb -> current_size += 1;

    pthread_cond_broadcast(&(bcb->can_get_orders));
    pthread_mutex_unlock(&(bcb->mutex)); 
    return order -> order_number;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
   if(bcb == NULL){
    return NULL;
   }

   pthread_mutex_lock(&(bcb->mutex));

   while(bcb -> current_size == 0 && bcb -> orders_handled < bcb -> expected_num_orders){
    pthread_cond_wait(&(bcb -> can_get_orders), &(bcb ->mutex));
   }


   if(bcb -> orders_handled >= bcb -> expected_num_orders){
    pthread_mutex_unlock(&(bcb->mutex));
    return NULL;
   }

   Order* popped_order = bcb -> orders;
   bcb -> orders = bcb -> orders -> next;
   bcb -> current_size--;
   bcb -> orders_handled++;
   if(bcb -> current_size == 0){
    pthread_cond_signal(&(bcb -> can_add_orders));
   }

   pthread_mutex_unlock(&(bcb -> mutex));
   return popped_order;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
  return false;
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  return false;
}

/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
    if (*orders == NULL){
        *orders = order;
        order -> next = NULL;
    } else {
        Order *current = *orders;
        while(current -> next != NULL){
            current = current -> next;
        }
        current -> next = order;
        order->next = NULL;
    }
}


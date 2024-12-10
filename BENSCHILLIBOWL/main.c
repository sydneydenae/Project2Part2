#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 10
#define NUM_COOKS 1
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
  int customer_id = (int)(long) tid;

  for(int i = 0; i < ORDERS_PER_CUSTOMER; i++){
    Order* new_order = (Order*) malloc(sizeof(Order));
    if(new_order == NULL){
    printf("Error: Could not allocate memory for order.\n");
    return NULL;
    }
  
  new_order -> menu_item = PickRandomMenuItem();
  new_order -> customer_id = customer_id;
  new_order -> next = NULL;

  int order_number = AddOrder(bcb, new_order);
  if(order_number == -1){
    printf("Error: Could not add order for customer %d.\n", customer_id);
    free(new_order);
    return NULL;
  }
  printf("Customer %d placed order %d.\n", customer_id, order_number);
}
  return NULL;
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = (int)(long) tid;
    int orders_fulfilled = 0;
    while(1){
      Order* order = GetOrder(bcb);

      if(order == NULL){
        if(bcb -> orders_handled >= bcb -> expected_num_orders){
          printf("Cook %d: No more orders to fulfil. Exiting the restauraunt...", cook_id);
          break;
        }
        continue;
      }
      printf("Cook %d: Fulfilling order #%d for customer %d: %s\n", cook_id, order -> order_number, order -> customer_id, order -> menu_item);
      free(order);
      orders_fulfilled++;
    }

	printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
	return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {

  bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
  if(bcb == NULL){
    printf("Error: Failed to open the restauraunt.\n");
    return -1;
  }

  // Create customer threads.
  pthread_t customer_threads[NUM_CUSTOMERS];
  for(int i =0; i < NUM_CUSTOMERS; i ++){
    pthread_create(&customer_threads[i], NULL, BENSCHILLIBOWLCustomer, (void*)(long)(i+1));
  }

  // Create cook threads.
  pthread_t cook_threads[NUM_COOKS];
  for(int i =0; i < NUM_COOKS; i ++){
    pthread_create(&cook_threads[i], NULL, BENSCHILLIBOWLCook, (void*)(long)(i+1));
  }

  // Wait for threads to finish
  for(int i =0; i < NUM_CUSTOMERS; i ++){
    pthread_join(customer_threads[i], NULL);
  }
  for(int i =0; i < NUM_COOKS; i ++){
    pthread_join(cook_threads[i], NULL);
  }

  CloseRestaurant(bcb);
  return 0;
}
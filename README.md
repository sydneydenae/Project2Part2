**Group Project: Ben's Chili Bowl**
<br>
Sydney Stokes and Charlie Seigler
<br>

You are to modify the file 
 and add the implementation for each of the functions listed below. If you have already started you will see a parameter based to all the functions of type BENSCHILLIBOWL *mcg. All references to *mcg should be changed to *bcb to reference the global variable declared in main.c

<br>

/*Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem();
/*Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant*/
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders);
/*check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources*/
void CloseRestaurant(BENSCHILLIBOWL* bcb);
/*add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order);
/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb);
/* Optional helper functions (you can implement if you think they would be useful)*/
bool IsFull(BENSCHILLIBOWL* bcb);
/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {}

#include <stdio.h>
#include "queue.h"

// Print out the index and the value of each element.
bool show_one(queue_element* elem, queue_function_args* args) {
  printf("Item %d == %d\n", *(int*) args, *(int*) elem);
  *(int*) args = *(int*) args + 1;
  return true;
}

typedef struct _order {
  int index;
  queue_element **expected;
  char *location;
  int *errors;
} order;

bool confirm_order(queue_element *elem, queue_function_args *args) {
  order *expected_order = (order*) args;
  int index = expected_order->index;
  int *errors = expected_order->errors;
  char *location = expected_order->location;
  queue_element *item = expected_order->expected[index];

  //  printf("item %d, element %d\n", *(int*) item, *(int*) elem);
  if (item != elem) {
    printf("Incorrect order in %s\n", location);
    printf("    at index %d expeced %d, got %d\n", index,
	   *(int*) item, *(int*) elem);
    (*errors)++;
  }

  expected_order->index++;
  return true;
}

void test_size(queue *q, int exp_size, char *location, int *errors) {
  size_t size = queue_size(q);
  if (size != exp_size) {
    printf("Queue size is incorrect in %s: expeceted %d, got %zu\n",
	   location, exp_size, size);
    (*errors)++;
  }
}

void confirm_append(queue *q, queue_element *elem, int exp_size,
		    char *location, int *errors) {
  queue_append(q, elem);
  test_size(q, exp_size, location, errors);
}

void test_append(int *errors) {
  char *location = "test_append";

  queue *q = queue_create();

  test_size(q, 0, location, errors);

  int x = 0;
  int y = 1;
  int z = 2;
  confirm_append(q, &x, 1, location, errors);
  confirm_append(q, &y, 2, location, errors);
  confirm_append(q, &z, 3, location, errors);
  confirm_append(q, &x, 4, location, errors);

  // Check the queue's order
  order expected_order;
  expected_order.index = 0;
  queue_element *expected[] = {&x, &y, &z, &x};
  expected_order.expected = expected;
  expected_order.location = location;
  expected_order.errors = errors;

  queue_apply(q, confirm_order, &expected_order);    
}

int main(int argc, char* argv[]) {
  int errors = 0;

  test_append(&errors);

  if (errors == 0)
    printf("All tests passed\n");
  else
    printf("\nTests done with %d error(s)\n", errors); 

  return 0;
}

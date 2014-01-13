#include <stdio.h>
#include "queue.h"

typedef struct _order {
  int index;
  queue_element** expected;
  char* location;
  int* errors;
} order;

bool confirm_order_fn(queue_element* elem, queue_function_args* args) {
  order* expected_order = (order*) args;
  int index = expected_order->index;
  int* errors = expected_order->errors;
  char* location = expected_order->location;
  queue_element* item = expected_order->expected[index];

  if (item != elem) {
    printf("Incorrect order in %s\n", location);
    printf("    at index %d expeced %d, got %d\n", index,
           *(int*) item, *(int*) elem);
    (*errors)++;
  }

  expected_order->index++;
  return true;
}

void confirm_order(queue* q, queue_element** expected,
                   char* location, int* errors) {
  order expected_order;
  expected_order.index = 0;
  expected_order.expected = expected;
  expected_order.location = location;
  expected_order.errors = errors;

  if (*errors > 0)
    printf("\n");  // Adds new line between each set

  queue_apply(q, confirm_order_fn, &expected_order);
}

void confirm_size(queue* q, int exp_size, char* location, int* errors) {
  size_t size = queue_size(q);
  if (size != exp_size) {
    printf("Queue size is incorrect in %s: expeceted %d, got %zu\n",
           location, exp_size, size);
    (*errors)++;
  }
}

void confirm_empty(queue* q, bool expected, char* location, int* errors) {
  if (queue_is_empty(q) != expected) {
    printf("Queue emptiness not as expected in %s: expeceted %d, got %d\n",
           location, expected, !expected);
    (*errors)++;
  }
}

void confirm_append(queue* q, queue_element* elem, int exp_size,
                    char* location, int* errors) {
  queue_append(q, elem);
  confirm_size(q, exp_size, location, errors);
}

// If exp_elem is NULL this will check test removing from an empty queue.
void confirm_remove(queue* q, queue_element* exp_elem, int exp_size,
                    char* location, int* errors) {
  queue_element* removed_elem;
  bool result = queue_remove(q, &removed_elem);
  confirm_size(q, exp_size, location, errors);

  if (exp_elem == NULL) {
    if (result) {
      int r_elem = *(int*) removed_elem;
      printf("Incorrect element removed in %s: expeceted 'Nothing', got %d\n",
             location, r_elem);
      (*errors)++;
    }
  } else {
    int r_elem = *(int*) removed_elem;
    int x_elem = *(int*) exp_elem;
    if (r_elem != x_elem) {
      printf("Incorrect element removed in %s: expeceted %d, got %d\n",
             location, x_elem, r_elem);
      (*errors)++;
    }
  }
}

void test_append(int* errors) {
  char* location = "test_append";

  queue* q = queue_create();

  confirm_size(q, 0, location, errors);

  int x = 0;
  int y = 1;
  int z = 2;
  confirm_append(q, &x, 1, location, errors);
  confirm_append(q, &y, 2, location, errors);
  confirm_append(q, &z, 3, location, errors);
  confirm_append(q, &x, 4, location, errors);

  // Check the queue's order
  /* order expected_order; */
  /* expected_order.index = 0; */
  /* queue_element* expected[] = {&x, &y, &z, &x}; */
  /* expected_order.expected = expected; */
  /* expected_order.location = location; */
  /* expected_order.errors = errors; */

  /* queue_apply(q, confirm_order, &expected_order); */

  queue_element* expected[] = {&x, &y, &z, &x};
  confirm_order(q, expected, location, errors);

  queue_destroy(q);
  q = NULL;
}

void test_remove(int* errors) {
  char* location = "test_remove";

  queue* q = queue_create();

  confirm_size(q, 0, location, errors);

  int x = 0;
  int y = 1;
  int z = 2;
  confirm_append(q, &x, 1, location, errors);
  confirm_append(q, &y, 2, location, errors);
  confirm_append(q, &z, 3, location, errors);
  confirm_append(q, &x, 4, location, errors);

  confirm_remove(q, &x, 3, location, errors);
  confirm_remove(q, &y, 2, location, errors);
  confirm_remove(q, &z, 1, location, errors);
  confirm_remove(q, &x, 0, location, errors);

  // Try to remove from empty queue
  confirm_remove(q, NULL, 0, location, errors);

  // Mix and match appending/removeing
  confirm_append(q, &y, 1, location, errors);
  confirm_append(q, &x, 2, location, errors);
  confirm_remove(q, &y, 1, location, errors);
  confirm_append(q, &z, 2, location, errors);
  confirm_remove(q, &x, 1, location, errors);
  confirm_remove(q, &z, 0, location, errors);
  confirm_remove(q, NULL, 0, location, errors);

  queue_destroy(q);
  q = NULL;
}

void test_is_empty(int* errors) {
  char* location = "test_is_empty";

  queue* q = queue_create();

  confirm_size(q, 0, location, errors);
  confirm_empty(q, true, location, errors);

  int x = 0;
  int y = 1;
  confirm_append(q, &x, 1, location, errors);
  confirm_empty(q, false, location, errors);
  confirm_append(q, &y, 2, location, errors);
  confirm_empty(q, false, location, errors);

  confirm_remove(q, &x, 1, location, errors);
  confirm_empty(q, false, location, errors);
  confirm_remove(q, &y, 0, location, errors);
  confirm_empty(q, true, location, errors);

  queue_destroy(q);
  q = NULL;
}

void test_reverse(int* errors) {
  char* location = "test_remove";

  queue* q = queue_create();

  confirm_size(q, 0, location, errors);

  int x = 0;
  int y = 1;
  int z = 2;
  confirm_append(q, &x, 1, location, errors);
  queue_element* expected1[] = {&x};
  confirm_order(q, expected1, location, errors);

  confirm_append(q, &y, 2, location, errors);
  queue_element* expected2[] = {&y, &x};
  confirm_order(q, expected2, location, errors);

  confirm_append(q, &z, 3, location, errors);
  queue_element* expected3[] = {&z, &x, &y};
  confirm_order(q, expected3, location, errors);

  confirm_append(q, &x, 4, location, errors);
  queue_element* expected4[] = {&x, &y, &x, &z};
  confirm_order(q, expected4, location, errors);

  queue_destroy(q);
  q = NULL;
}

int main(int argc, char* argv[]) {
  int errors = 0;

  test_append(&errors);
  test_remove(&errors);
  test_is_empty(&errors);
  test_reverse(&errors);

  if (errors == 0)
    printf("All tests passed\n");
  else
    printf("\nTests done with %d error(s)\n", errors);

  return 0;
}

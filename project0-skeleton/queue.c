/* Implements queue abstract data type. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/* Each link in the queue stores a queue_element and
 * a pointer to the next link in the queue. */
typedef struct _queue_link {
  queue_element* elem;
  struct _queue_link* next;
} queue_link;

/* This is the actual implementation of the queue struct that
 * is declared in queue.h. */
struct _queue {
  queue_link* head;
};

/* Private */
void _check_alloc(void* ptr) {
  if (ptr == NULL) {
    printf("Memory Error\n");
    exit(1);
  }
}

queue* queue_create() {
  queue* q = (queue*) malloc(sizeof(queue));
  _check_alloc(q);

  q->head = NULL;
  return q;
}

// Lets the user clean up to prevent memory leaks.
void queue_destroy(queue* q) {
  queue_element* elem;
  while (queue_remove(q, &elem)) {}
  free(q);
}

/* Private */
static queue_link* queue_new_element(queue_element* elem) {
  queue_link* ql = (queue_link*) malloc(sizeof(queue_link));
  _check_alloc(ql);

  ql->elem = elem;
  ql->next = NULL;

  return ql;
}

void queue_append(queue* q, queue_element* elem) {
  assert(q != NULL);

  // Need to handle the case where the queue is empty.
  if (queue_is_empty(q)) {
    q->head = queue_new_element(elem);
  } else {
    // Find the last link in the queue.
    queue_link* cur;
    for (cur = q->head; cur->next; cur = cur->next) {}

    // Append the new link.
    cur->next = queue_new_element(elem);
  }
}

bool queue_remove(queue* q, queue_element** elem_ptr) {
  queue_link* old_head;

  assert(q != NULL);
  assert(elem_ptr != NULL);
  if (queue_is_empty(q)) {
    return false;
  }

  *elem_ptr = q->head->elem;
  old_head = q->head;
  q->head = q->head->next;

  // Prevents leaking memory.
  free(old_head);

  return true;
}

bool queue_is_empty(queue* q) {
  assert(q != NULL);
  return q->head == NULL;
}

/* private */
static bool queue_count_one(queue_element* elem, queue_function_args* args) {
  size_t* count = (size_t*) args;
  *count = *count + 1;
  return true;
}

size_t queue_size(queue* q) {
  size_t count = 0;
  queue_apply(q, queue_count_one, &count);
  return count;
}

bool queue_apply(queue* q, queue_function qf, queue_function_args* args) {
  assert(q != NULL && qf != NULL);

  if (queue_is_empty(q))
    return false;

  for (queue_link* cur = q->head; cur; cur = cur->next) {
    if (!qf(cur->elem, args))
      break;
  }

  return true;
}

/* private */
queue_link* get_ith(queue* q, int i, queue_link** prev) {
  int current = 0;
  for (queue_link* cur = q->head; cur; cur = cur->next) {
    if (current == i)
      return cur;

    current++;
    *prev = cur;
  }

  return NULL;  // i >= q length
}

void queue_reverse(queue* q) {
  assert(q != NULL);

  // Trivial cases
  if (queue_is_empty(q) || queue_size(q) <= 1)
    return;

  // for i = 1 to q length: move ith element to front
  for (int i = 1; i < queue_size(q); ++i) {
    queue_link* prev_ith;
    queue_link* ith = get_ith(q, i, &prev_ith);
    // All ith nodes have a prev because we start from i = 1
    prev_ith->next = ith->next;
    ith->next = q->head;
    q->head = ith;
  }
}

/* private */
// If start is the largest then prev will be unchanged
queue_link* get_largest(queue_link* start, queue_compare qc,
                        queue_link** prev) {
  // cur_prev is always the node before cur,
  // prev is always the node before largest
  queue_link* largest = start;
  queue_link* cur_prev = *prev;
  for (queue_link* cur = start; cur; cur = cur->next) {
    if (qc(cur->elem, largest->elem) == 1) {
      largest = cur;
      *prev = cur_prev;
    }

    cur_prev = cur;
  }

  return largest;
}

void queue_sort(queue* q, queue_compare qc) {
  assert(q != NULL);

  // Trivial cases
  if (queue_is_empty(q) || queue_size(q) <= 1)
    return;

  // insertion sort - move 'largest' elements to front from unsorted section
  queue_link* unsorted_head = q->head;
  // prev_largest is set to the node before unsorted_head prior to call to
  // get_largest (which happens to always be the absolute largest node), and
  // afterward is set to the node before the next_largest so that we can 'cut
  // it out'
  queue_link* prev_largest = NULL;
  queue_link* absolute_largest = NULL;
  bool update_absolute_largest = true;

  while (unsorted_head != NULL) {
    // Incase next_largest will be the same as unsorted_head, the prev_largest
    // will be the absolute_largest
    prev_largest = absolute_largest;
    queue_link* next_largest = get_largest(unsorted_head, qc, &prev_largest);

    // We find out the largest on the first time through
    if (update_absolute_largest) {
      absolute_largest = next_largest;
      update_absolute_largest = false;
    }

    // The largest might already be at the unsorted_start, which is the only
    // time we advance the unsorted_head
    if (next_largest == unsorted_head) {
      unsorted_head = unsorted_head->next;
    }

    // Move largest to front
    prev_largest->next = next_largest->next;
    next_largest->next = q->head;
    q->head = next_largest;
  }
}

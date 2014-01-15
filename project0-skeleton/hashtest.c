#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

static const size_t kBufferLength = 32;
static const uint32_t kMaxInsertions = 100000;
static const char kNotFoundKey[] = "not-found key";

/* Matches the hash_hasher definition in hash.h: */
static uint64_t hash_fn(const void* k) {
  uint64_t hash_val = 0;
  uint64_t coefficient = 1;

  for (const char* p = (const char*) k; *p != '\0'; p++) {
    hash_val += coefficient * (*p);
    coefficient *= 37;
  }

  return hash_val;
}

/* Matches the hash_compare definition in hash.h. This function compares
 * two keys that are strings. */
static int hash_strcmp(const void* k1, const void* k2) {
  return strcmp((const char*) k1, (const char*) k2);
}

void confirm_insert(hash_table* ht, char *k, int64_t *v,
                    char* exp_rk, int64_t* exp_rv,
                    char* location, int* errors) {
  char* removed_key = NULL;
  int64_t* removed_value = NULL;
  hash_insert(ht, k, v, (void**) &removed_key, (void**) &removed_value);

  if (((removed_key == NULL || exp_rk == NULL) && removed_key != exp_rk) ||
      (removed_key != NULL && exp_rk != NULL &&
       strcmp(removed_key, exp_rk) != 0)) {
    printf("Incorrect key removed during insert in %s: expeceted %s, got %s\n",
           location, exp_rk, removed_key);
    (*errors)++;
  }
  if (*removed_value != *removed_value) {
    printf("Inserted (%s, %" PRIi64 ")\n", k, *v);
    printf("Incorrect value replaced during insert in %s: expeceted %"
           PRIi64 ", got %" PRIi64 "\n",
           location, *exp_rv, *removed_value);
    (*errors)++;
  }
}

void confirm_lookup(hash_table* ht, char *k, int64_t* exp_rv, bool exp,
                    char* location, int* errors) {
  int64_t* value;
  bool result = hash_lookup(ht, k, (void**) &value);

  if (result != exp) {
    if (result) {
      printf("Lookup error in %s: expeceted table to not conatin %s, "
             "but it did\n", location, k);
    } else {
      printf("Lookup error in %s: expeceted table to conatin %s, "
             "but it did not\n", location, k);
    }
    (*errors)++;
  }

  if (exp) {
    if (value == NULL) {
      printf("Lookup error in %s: expeceted %" PRIi64 ", got (null)\n",
             location, *exp_rv);
    } else if (*value != *exp_rv) {
      printf("Lookup error in %s: expeceted %" PRIi64 ", got %" PRIi64 "\n",
             location, *exp_rv, *value);
    }
  }
  (*errors)++;
}

void test_insert(int* errors) {
  char* location = "test_insert";

  hash_table* ht = hash_create(hash_fn, hash_strcmp);

  char* k1 = "key1";
  int64_t v1 = 1;
  confirm_insert(ht, k1, &v1, NULL, NULL, location, errors);

  char* k2 = "key2";
  int64_t v2 = 2;
  confirm_insert(ht, k2, &v2, NULL, NULL, location, errors);

  int64_t v3 = 3;
  confirm_insert(ht, k1, &v3, k1, &v1, location, errors);
}

void test_lookup(int* errors) {
  char* location = "test_lookup";

  hash_table* ht = hash_create(hash_fn, hash_strcmp);

  char* k0 = "key0";
  confirm_lookup(ht, k0, NULL, false, location, errors);

  char* k1 = "key1";
  int64_t v1 = 1;
  confirm_insert(ht, k1, &v1, NULL, NULL, location, errors);

  char* k2 = "key2";
  int64_t v2 = 2;
  confirm_insert(ht, k2, &v2, NULL, NULL, location, errors);

  char* k3 = "key3";
  int64_t v3 = 3;
  confirm_insert(ht, k3, &v3, NULL, NULL, location, errors);

  confirm_lookup(ht, k1, &v1, true, location, errors);
  confirm_lookup(ht, k2, &v2, true, location, errors);
  confirm_lookup(ht, k3, &v3, true, location, errors);

  char* k4 = "key4";
  confirm_lookup(ht, k4, NULL, false, location, errors);
}

void test_present(int* errors) {
  // Check for empty table
  // Add a few values
  // Check for added values
  // Check for value that wasn't added
}

void test_remove(int* errors) {
  // Check for empty table
  // Add one item and check
  // Add two items and check between each remove
}

void test_destroy(int* errors) {
  // Add stack values and destroy
  // Add malloced values and destroy
  // Let valgrind check memory
}

int main(int argc, char* argv[]) {
  int errors = 0;

  test_insert(&errors);
  test_lookup(&errors);
  /* test_present(&errors); */
  /* test_remove(&errors); */
  /* test_destroy(&errors); */

  if (errors == 0)
    printf("All tests passed\n");
  else
    printf("\nTests done with %d error(s)\n", errors);

  /* /\* Check for correct invocation: *\/ */
  /* if (argc != 2) { */
  /*   printf("Usage: %s <N>\n" */
  /*       "Run test inserting a total of N items\n", argv[0]); */
  /*   return 1; */
  /* } */
  /* int N = atoi(argv[1]); */
  /* if (N <= 0 || N > kMaxInsertions) { */
  /*   N = kMaxInsertions; */
  /* } */

  /* /\* Create the hash table. *\/ */
  /* hash_table* ht = hash_create(hash_fn, hash_strcmp); */

  /* /\* First phase: insert some data. *\/ */
  /* printf("\nInsert phase:\n"); */
  /* char* k; */
  /* int64_t* v; */
  /* char* removed_key = NULL; */
  /* int64_t* removed_value = NULL; */
  /* for (int i = 0; i < N * 2; i++) { */
  /*   k = (char*) malloc(kBufferLength); */
  /*   snprintf(k, kBufferLength, "String %d", i % N); */
  /*   v = (int64_t*) malloc(sizeof(int64_t)); */
  /*   *v = i; */
  /*   // The hash map takes ownership of the key and value: */
  /*   hash_insert(ht, k, v, (void**) &removed_key, (void**) &removed_value); */
  /*   if (removed_value != NULL) { */
  /*     printf("Replaced (%s, %" PRIi64 ") while inserting (%s, %" PRIi64 ")\n", */
  /*            removed_key, *removed_value, k, *v); */
  /*     free(removed_key); */
  /*     free(removed_value); */
  /*   } else { */
  /*     printf("Inserted (%s, %" PRIi64 ")\n", k, *v); */
  /*   } */
  /* } */

  /* /\* Second phase: look up some data. *\/ */
  /* printf("\nLookup phase:\n"); */
  /* char strbuf[kBufferLength]; */
  /* for (int i = N - 1; i >= 0; i--) { */
  /*   snprintf(strbuf, kBufferLength, "String %d", i); */
  /*   if (!hash_lookup(ht, strbuf, (void**) &v)) { */
  /*     printf("Entry for %s not found\n", strbuf); */
  /*   } else { */
  /*     printf("%s -> %" PRIi64 "\n", strbuf, *v); */
  /*   } */
  /* } */

  /* /\* Look up a key that hasn't been inserted: *\/ */
  /* if (!hash_lookup(ht, kNotFoundKey, (void**) &v)) { */
  /*   printf("Lookup of \"%s\" failed (as expected)\n", kNotFoundKey); */
  /* } else { */
  /*   printf("%s -> %" PRIi64 " (unexpected!)\n", kNotFoundKey, *v); */
  /* } */

  /* /\* Destroy the hash table and free things that we've allocated. Because */
  /*  * we allocated both the keys and the values, we instruct the hash map */
  /*  * to free both. */
  /*  *\/ */
  /* hash_destroy(ht, true, true); */

  return 0;
}

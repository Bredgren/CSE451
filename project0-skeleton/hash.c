/* Implements the abstract hash table. */

#include <assert.h>
#include <stdlib.h>

#include "hash.h"

/* Using an entry of this type is just a suggestion. You're
 * free to change this. */
typedef struct _hash_entry {
  void* key;
  void* value;
  struct _hash_entry* next;
} hash_entry;

struct _hash_table {
  int num_buckets;
  hash_entry* buckets;
};

static const int kDefaultSize = 32;

hash_table* hash_create(hash_hasher h, hash_compare c) {
  hash_entry* buckets = (hash_entry*) malloc(sizeof(hash_entry) * kDefaultSize);
  hash_table* ht = (hash_table*) malloc(sizeof(hash_table));

  ht->num_buckets = 0;
  ht->buckets = buckets;

  return ht;
}

void hash_insert(hash_table* ht, void* key, void* value,
                 void** removed_key_ptr, void** removed_value_ptr) {

}

bool hash_lookup(hash_table* ht, const void* key, void** value_ptr) {
  return false;
}

bool hash_is_present(hash_table* ht, const void* key) {
  return false;
}

bool hash_remove(hash_table* ht, const void* key,
                 void** removed_key_ptr, void** removed_value_ptr) {
  return false;
}

void hash_destroy(hash_table* ht, bool free_keys, bool free_values) {

}

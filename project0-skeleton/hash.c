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
  hash_hasher hasher;
  hash_compare compare;
  hash_entry* buckets;
};

static const int kDefaultSize = 32;

hash_table* hash_create(hash_hasher h, hash_compare c) {
  hash_entry* buckets = (hash_entry*) malloc(sizeof(hash_entry) * kDefaultSize);
  hash_table* ht = (hash_table*) malloc(sizeof(hash_table));

  ht->num_buckets = 0;
  ht->buckets = buckets;
  ht->hasher = h;
  ht->compare = c;

  return ht;
}

/* Private */
int _get_bucket(hash_table* ht, const void* key) {
  return ht->hasher(key) % ht->num_buckets;
}

/* Private */
hash_entry* _find_entry(hash_table* ht, const void* key, hash_entry** prev) {
  // TODO
  return NULL
}

void hash_insert(hash_table* ht, void* key, void* value,
                 void** removed_key_ptr, void** removed_value_ptr) {
  // TODO if (time for resize) ...

  if (hash_is_present(ht, key)) {

  } else {
    int bucket_num = _get_bucket(ht, key);
    hash_entry* bucket = &ht->buckets[bucket_num];

    hash_entry* new_entry = (hash_entry*) malloc(sizeof(hash_entry));
    new_entry->key = key;
    new_entry->value = value;

      if (bucket == NULL) {
        bucket = new_entry;
      } else {
        // Just add it to the front
        new_entry->next = bucket;
        bucket = new_entry;
      }
  }
}

bool hash_lookup(hash_table* ht, const void* key, void** value_ptr) {
  hash_entry* prev;
  hash_entry* entry = _find_entry(ht, key, &prev);
  *value_ptr = entry->value;
  return entry != NULL;
}

bool hash_is_present(hash_table* ht, const void* key) {
  int bucket_num = _get_bucket(ht, key);
  hash_entry* bucket = &ht->buckets[bucket_num];

  // TODO
  // iterate over bucket to see if key is there

  return false;
}

bool hash_remove(hash_table* ht, const void* key,
                 void** removed_key_ptr, void** removed_value_ptr) {
  // TODO
  return false;
}

void hash_destroy(hash_table* ht, bool free_keys, bool free_values) {
  if (free_keys) {
    // TODO
  }

  if (free_values) {
    // TODO
  }

  free(ht->buckets);
  free(ht);
}

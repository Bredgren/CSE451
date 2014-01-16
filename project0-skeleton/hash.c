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
  hash_entry** buckets;
};

static const int kDefaultSize = 32;

hash_table* hash_create(hash_hasher h, hash_compare c) {
  int size = sizeof(hash_entry);
  // Without calloc old tables are 'resurected' with bogus values. Example:
  // the second table my tests make aleady had an entry with 'key1' but didn't
  // have 1 as a value.
  hash_entry** buckets = (hash_entry**) calloc(kDefaultSize, size);
  hash_table* ht = (hash_table*) malloc(sizeof(hash_table));

  ht->num_buckets = kDefaultSize;
  ht->buckets = buckets;
  ht->hasher = h;
  ht->compare = c;

  return ht;
}

/* Private */
/* Returns a pointer to the first entry in the bucket that the given key
   belongs to, also sets bucket_num to be the number of the bucket. */
hash_entry*  _get_bucket(hash_table* ht, const void* key, int* bucket_num) {
  *bucket_num = ht->hasher(key) % ht->num_buckets;
  return ht->buckets[*bucket_num];
}

/* Private */
/* Returns a pointer to the entry whose key matches the given one, it also
   sets prev to be the entry before the returned one (*prev->next = returned),
   so it will be NULL if there are 0 or 1 entries in the bucket it came from. */
hash_entry* _find_entry(hash_table* ht, const void* key, hash_entry** prev) {
  int num;
  hash_entry* bucket = _get_bucket(ht, key, &num);
  *prev = NULL;

  for (; bucket != NULL; bucket = bucket->next) {
    if (ht->compare(bucket->key, key) == 0)
      return bucket;
    *prev = bucket;
  }

  return NULL;
}

void hash_insert(hash_table* ht, void* key, void* value,
                 void** removed_key_ptr, void** removed_value_ptr) {
  // TODO if (time for resize) ...

  if (hash_is_present(ht, key)) {

  } else {
    int bucket_num;
    hash_entry* bucket = _get_bucket(ht, key, &bucket_num);

    hash_entry* new_entry = (hash_entry*) malloc(sizeof(hash_entry));
    new_entry->key = key;
    new_entry->value = value;

    // Just add it to the front
    if (bucket != NULL) {
      new_entry->next = bucket;
    }
    ht->buckets[bucket_num] = new_entry;
  }
}

bool hash_lookup(hash_table* ht, const void* key, void** value_ptr) {
  hash_entry* prev;
  hash_entry* entry = _find_entry(ht, key, &prev);
  if (entry != NULL)
    *value_ptr = entry->value;
  return entry != NULL;
}

bool hash_is_present(hash_table* ht, const void* key) {
  void* v;
  return hash_lookup(ht, key, &v);
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

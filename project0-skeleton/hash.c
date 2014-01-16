/* Implements the abstract hash table. */

#include <assert.h>
#include <stdio.h>
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
  int num_entries;
  hash_hasher hasher;
  hash_compare compare;
  hash_entry** buckets;
};

static const int kDefaultSize = 32;
static const int kResizeFactor = 10;

/* Private */
void _check_alloc(void* ptr) {
  if (ptr == NULL) {
    printf("Memory Error\n");
    exit(1);
  }
}

hash_table* hash_create(hash_hasher h, hash_compare c) {
  int size = kDefaultSize;
  // Without calloc old tables are 'resurected' with bogus values. Example:
  // the second table my tests make aleady had an entry with 'key1' but didn't
  // have 1 as a value.
  hash_entry** buckets = (hash_entry**) calloc(size, sizeof(hash_entry));
  _check_alloc(buckets);
  hash_table* ht = (hash_table*) malloc(sizeof(hash_table));
  _check_alloc(ht);

  ht->num_buckets = size;
  ht->num_entries = 0;
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

/* Private */
/* Doubles the size of the given hash table. */
void _resize(hash_table* ht) {
  int size = ht->num_buckets * 2;
  hash_entry** new_buckets = (hash_entry**) calloc(size, sizeof(hash_entry));
  _check_alloc(new_buckets);
  hash_entry** old_buckets = ht->buckets;
  int num_old_buckets = ht->num_buckets;
  ht->buckets = new_buckets;
  ht->num_buckets = size;

  for (int i = 0; i < num_old_buckets; ++i) {
    for (hash_entry* entry = old_buckets[i]; entry != NULL;
         entry = entry->next) {
      void *rk, *rv;
      hash_insert(ht, entry->key, entry->value, &rk, &rv);
      free(entry);
    }
  }
  free(old_buckets);
}

void hash_insert(hash_table* ht, void* key, void* value,
                 void** removed_key_ptr, void** removed_value_ptr) {
  if (ht->num_entries >= ht->num_buckets * kResizeFactor) {
    _resize(ht);
  }

  if (hash_is_present(ht, key)) {
    hash_entry* prev;
    hash_entry* entry = _find_entry(ht, key, &prev);
    *removed_key_ptr = entry->key;
    *removed_value_ptr = entry->value;
    entry->key = key;
    entry->value = value;
  } else {
    int bucket_num;
    hash_entry* bucket = _get_bucket(ht, key, &bucket_num);

    hash_entry* new_entry = (hash_entry*) malloc(sizeof(hash_entry));
    _check_alloc(new_entry);
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = NULL;  // Apparently this is required by valgrind

    // Just add it to the front
    if (bucket != NULL) {
      new_entry->next = bucket;
    }
    ht->buckets[bucket_num] = new_entry;

    ht->num_entries++;
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
  if (!hash_is_present(ht, key))
    return false;

  hash_entry* prev;
  hash_entry* entry = _find_entry(ht, key, &prev);

  if (prev) {
    prev->next = entry->next;
  } else {
    int bucket_num;
    _get_bucket(ht, key, &bucket_num);
    ht->buckets[bucket_num] = NULL;
  }

  *removed_key_ptr = entry->key;
  *removed_value_ptr = entry->value;

  free(entry);
  ht->num_entries--;

  return true;
}

void hash_destroy(hash_table* ht, bool free_keys, bool free_values) {
  for (int i = 0; i < ht->num_buckets; ++i) {
    for (hash_entry* entry = ht->buckets[i]; entry != NULL;) {
      if (free_keys)
        free(entry->key);
      if (free_values)
        free(entry->value);

      hash_entry* old = entry;
      entry = entry->next;
      free(old);
    }
  }

  free(ht->buckets);
  free(ht);
}

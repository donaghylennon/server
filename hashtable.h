#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>

#define INITIAL_SIZE 64

struct bucket {
    const char *key;
    const char *value;
    struct bucket *next;
    struct bucket *prev;
};

typedef struct {
    struct bucket **buckets;
    size_t capacity;
    size_t size;
} HashTable;

HashTable *hashtable_create();
void hashtable_destroy(HashTable *h);
const char *hashtable_get(HashTable *h, const char *key);
struct bucket *hashtable_get_bucket(HashTable *h, const char *key);
void hashtable_put(HashTable *h, const char *key, const char *value);
void hashtable_remove(HashTable *h, const char *key);

struct bucket *bucket_create(const char *key, const char *value);
void bucket_destroy(struct bucket *b);

#endif

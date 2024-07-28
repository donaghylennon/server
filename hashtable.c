#include "hashtable.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static uint64_t hash_key(const char *key) {
    // Borrowed from https://benhoyt.com/writings/hash-table-in-c/
    uint64_t hash = FNV_OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)*p;
        hash *= FNV_PRIME;
    }
    return hash;
}

HashTable *hashtable_create() {
    HashTable *h = (HashTable *)malloc(sizeof(HashTable));
    h->buckets = (struct bucket **)malloc(INITIAL_SIZE * sizeof(struct bucket *));
    memset(h->buckets, 0, INITIAL_SIZE * sizeof(struct bucket *));
    h->capacity = INITIAL_SIZE;
    h->size = 0;
    return h;
}

void hashtable_destroy(HashTable *h) {
    for (size_t i = 0; i < h->capacity; i++) {
        struct bucket *p = h->buckets[i];
        if (p != NULL) {
            for (struct bucket *q = p; q != NULL;) {
                struct bucket *r = q;
                q = q->next;
                if (r->key)
                    free((void *)r->key);
                if (r->value)
                    free((void *)r->value);
                free(r);
            }
        }
    }
}

const char *hashtable_get(HashTable *h, const char *key) {
    if (!key)
        return NULL;
    uint64_t hash = hash_key(key);
    size_t i = hash % h->capacity;

    if (h->buckets[i]) {
        for (struct bucket *p = h->buckets[i]; p != NULL; p++) {
            if (!strcmp(key, p->key))
                return p->value;
        }
    }
    return NULL;
}

struct bucket *hashtable_get_bucket(HashTable *h, const char *key) {
    if (!key)
        return NULL;
    uint64_t hash = hash_key(key);
    size_t i = hash % h->capacity;

    if (h->buckets[i]) {
        for (struct bucket *p = h->buckets[i]; p != NULL; p++) {
            if (!strcmp(key, p->key))
                return p;
        }
    }
    return NULL;
}

void hashtable_put(HashTable *h, const char *key, const char *value) {
    // TODO: resize hash table when size exceeds certain threshold
    if (!h || !key || !value)
        return;
    uint64_t hash = hash_key(key);
    size_t i = hash % h->capacity;

    struct bucket *new_bucket = bucket_create(key, value);
    h->size++;
    struct bucket *head_bucket = h->buckets[i];
    h->buckets[i] = new_bucket;
    if (head_bucket) {
        new_bucket->next = head_bucket;
        head_bucket->prev = new_bucket;
        for (struct bucket *b = head_bucket; b != NULL; b = b->next) {
            if (!strcmp(b->key, key)) {
                bucket_destroy(b);
                h->size--;
                break;
            }
        }
    }
}

void hashtable_remove(HashTable *h, const char *key) {
    // TODO: resize hash table when size goes below certain threshold
    if (!h || !key)
        return;
    uint64_t hash = hash_key(key);
    size_t i = hash % h->capacity;

    if (h->buckets[i]) {
        if (!strcmp(key, h->buckets[i]->key)) {
            struct bucket *p = h->buckets[i];
            h->buckets[i] = h->buckets[i]->next;
            bucket_destroy(p);
            h->size -= 1;
        } else {
            for (struct bucket *p = h->buckets[i]->next; p != NULL; p++) {
                if (!strcmp(key, p->key)) {
                    bucket_destroy(p);
                    h->size -= 1;
                    break;
                }
            }
        }
    }
}

struct bucket *bucket_create(const char *key, const char *value) {
    if (!key || !value)
        return NULL;
    size_t key_size = strlen(key) + 1;
    size_t value_size = strlen(value) + 1;
    char *key_copy = (char *)malloc(key_size * sizeof(char));
    char *value_copy = (char *)malloc(value_size * sizeof(char));
    strcpy(key_copy, key);
    strcpy(value_copy, value);

    struct bucket *b = (struct bucket *)malloc(sizeof(struct bucket));
    b->key = key_copy;
    b->value = value_copy;
    b->next = NULL;
    b->prev = NULL;

    return b;
}

void bucket_destroy(struct bucket *b) {
    if (!b)
        return;
    if (b->key)
        free((void *)b->key);
    if (b->value)
        free((void *)b->value);
    if (b->next)
        b->next->prev = b->prev;
    if (b->prev)
        b->prev->next = b->next;
    free(b);
}

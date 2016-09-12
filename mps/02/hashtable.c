#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/* Daniel J. Bernstein's "times 33" string hash function, from comp.lang.C;
   See https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk */
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

hashtable_t *make_hashtable(unsigned long size) {
  hashtable_t *ht = malloc(sizeof(hashtable_t));
  ht->size = size;
  ht->buckets = calloc(sizeof(bucket_t *), size);
  return ht;
}

void ht_put(hashtable_t *ht, char *key, void *val) {
  unsigned int idx = hash(key) % ht->size;

  bucket_t *temp = ht->buckets[idx];
  while (temp) {
    if (strcmp(temp->key, key) == 0) {
      temp->val = val;
      return;
    }
    temp = temp->next;
  }

  bucket_t *b = malloc(sizeof(bucket_t));
  b->key = key;
  b->val = val;
  b->next = ht->buckets[idx];
  ht->buckets[idx] = b;
}

void *ht_get(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return b->val;
    }
    b = b->next;
  }
  return NULL;
}

void ht_iter(hashtable_t *ht, int (*f)(char *, void *)) {
  bucket_t *b;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      if (!f(b->key, b->val)) {
        return ; // abort iteration
      }
      b = b->next;
    }
  }
}

void free_hashtable(hashtable_t *ht) {
  unsigned long i;
  bucket_t *p, *q;
  for (i=0; i<ht->size; i++) {
    p=ht->buckets[i];
    while(p) {
      q=p->next;
      p=q;
    }
  }
  free(ht); // FIXME: must free all substructures!
}

void  ht_del(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *temp, *b = ht->buckets[idx];
  if (strcmp(b->key, key) == 0) {
    temp = b->next;
    ht->buckets[idx] = temp;
    return;
  }
  while(b->next) {
    if (strcmp(b->next->key, key) == 0) {
      temp = b->next->next;
      b->next=temp;
      return;
    }
    b=b->next;
  }
}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
  hashtable_t *old=ht;
  ht=make_hashtable(newsize);
  bucket_t *b;
  unsigned long i;
  for (i=0; i<old->size; i++) {
    b = old->buckets[i];
    while(b) {
      ht_put(ht, b->key, b->val);
      b = b->next;
    }
  }
  old->size = newsize;
  old->buckets = ht->buckets; 
}

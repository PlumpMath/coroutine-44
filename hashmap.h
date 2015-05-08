/* You should always use 1024 */
#ifndef   HASHMAP_H_INCLUDED
#define   HASHMAP_H_INCLUDED

#include "list.h"
typedef struct hash_map_t
{
    int size; 
    int slot_size;
    list* slot;
    
}hash_map;
typedef struct  pair_t
{
    void *key;
    void *val;
}pair;


int hmap_create(hash_map* hmap);
void hmap_destroy(hash_map* hmap);
void hmap_expand(hash_map* hmap);

int hmap_insert(hash_map* hmap, void* key, void* value);

int hmap_size(hash_map* hmap);
int hmap_remove(hash_map* hmap, void* key);

void*  hmap_find(hash_map* hmap, void* key);

#endif  /* HASHMAP_H_INCLUDED */

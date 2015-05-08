#include "hashmap.h"
#include <stdlib.h>
int hmap_create(hash_map* hmap)
{
    if(hmap != NULL)
    {
        hmap->size = 0;
        hmap->slot_size = 100;
        hmap->slot = (list*)malloc(sizeof(list) *  hmap->slot_size);
        if(hmap->slot  == NULL) return -1; 
        return 0;
    }
    return -1;
}

/* Destroy after use */
void hmap_destroy(hash_map* hmap)
{
    if(hmap == NULL)return ;
    int i ;
    for(i = 0; i< hmap->slot_size; i ++);
    {
        list_destroy(&hmap->slot[i]);
    }    
    free(hmap);
}

void hmap_expand(hash_map* hmap)
{
    int tmp = hmap->slot_size > 2 + hmap->slot_size;
    list* slot = (list*)malloc(sizeof(list)* tmp);
    if(slot == NULL) return;
    int i = 0;
    for(i=0; i< hmap->slot_size; i++)
    {
        while(list_size(&hmap->slot[i]) > 0)
        {
            list_add_tail(&slot[i], list_get_tail(&hmap->slot[i]));
        }
    }
     
}


/* Insert a key-value into hash map. value is a pointer to callee-allocated memory */
int hmap_insert(hash_map* hmap, void* key, void* value)
{
    if(hmap == NULL) return -1;
    pair* data = (pair*)malloc(sizeof(pair));
    if(data == NULL) return -1;
    data->key  = key;
    data->val = value;  
    if(hmap_find(hmap, key) != NULL)hmap_remove(hmap, key);
    int id = (int)key % hmap->slot_size;
    int ret =  list_add_tail(&hmap->slot[id], data);
    /*
    if (ret > 0)
    {
        hmap->size++;
        if(hmap->size / hmap->slot_size > 10)
        {
            hmap_expand(hmap);     
        }
    }*/
    return ret;
}


int hmap_size(hash_map* hmap)
{
    if(hmap == NULL)return 0;
    return hmap->size;
}
int hmap_remove(hash_map* hmap, void* key)
{
    if(hmap == NULL)return -1;
    int id = (int)key % hmap->slot_size;
    node* n = hmap->slot[id].head;
    while(n != NULL)
    {
        pair* data = n->data;
        if(data->key == key)
        {
            return list_remove(&hmap->slot[id], data);
        } 
        n= n->next;
    }
    return -1;
}

/* Find a hash map for value of given key string */
void*  hmap_find(hash_map* hmap, void* key)
{
    if(hmap == NULL) return NULL;
    int id =  (int)key % hmap->slot_size;
    node* n = hmap->slot[id].head;
    while(n != NULL)
    {
        pair* data = n->data;
        if(data->key == key) return data->val;
        n = n->next;
    }
    return NULL;
}


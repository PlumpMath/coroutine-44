#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

typedef struct node_t
{
    struct node_t *pre;
    struct node_t *next;
    void* data;
}node;

typedef struct list_t
{
    int size;    /* count of nodes */
    node *head;
    node *tail;
}list;

extern void list_create(list* head);
extern void list_destroy(list* head);

/* Appends a node to a head */
extern int list_add_head(list *list_head, void* data);

/* Appends a node to a list */
extern int list_add_tail(list *list_head, void* data);

/* Removes the first node from a list and returns it */
extern void* list_get_head(list *list_head);

/* Removes the first node from a list and returns it */
extern void* list_get_tail(list *list_head);

extern int list_remove(list* list_head, void* data);
extern int list_size(list* list_head);


#endif  /* LIST_H_INCLUDED */
 

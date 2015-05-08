#include "list.h"
#include <stdlib.h>

void list_init(list* head)
{
    if(head != NULL)
    {
        head->size =0;
        head->head = NULL;
        head->tail =NULL;
    }
}

void list_destroy(list* head)
{
    if(head == NULL)return;
    while(head->size >0)
    {
        list_get_tail(head);
    }
}

int list_add_head(list *list_head, void* data)
{
    if(list_head != NULL)
    {
        node* head=(node*)malloc(sizeof(node));
        if(head == NULL)return -1;
        head->data = data;
        head->pre = NULL;
        if(list_head->head == NULL)
        {
            list_head->head = head;
            list_head->tail = head;
            head->next = head;
            head->pre = head;
            list_head->size++;
            return 0;
        }
        head->next = list_head->head;
        list_head->head->pre = head;
        list_head->head = head;
        list_head->size++;
        return 0;
    } 
    return -1;
}

int  list_add_tail(list *list_head, void* data)
{
    if(list_head != NULL)
    {
        node* tail = (node*)malloc(sizeof(node));
        if(tail == NULL)return -1;
        tail->data = data;
        tail->next = NULL;
        if(list_head->tail == NULL)
        {
            list_head->head = tail;
            list_head->tail = tail;
            tail->next = tail;
            tail->pre = tail; 
            list_head->size++;
            return 0;
        }
        tail->pre = list_head->tail;
        list_head->tail->next = tail;
        list_head->tail = tail;
        list_head->size++;
        return 0;
    }
    return -1;    
}


int list_size(list* head)
{
    return head->size;
}


void* list_get_head(list *list_head)
{
    if(list_head != NULL)
    {
        node* head = list_head->head;
        void* data = head->data;
        list_head->head = head->next;
        list_head->head->pre = NULL;
        list_head->size --;
        free(head);
        return data;
    }
    return NULL;
}


void* list_get_tail(list* list_head)
{
    if(list_head != NULL)
    {
        node* tail = list_head->tail;
        void* data = tail->data;
        list_head->tail = tail->pre;
        list_head->tail->next = NULL;
        list_head->size --;
        free(tail);
        return data;
    }
    return NULL;
}


extern int list_remove(list* list_head, void* data)
{
    if(list_head == NULL)return -1;
    if (data == list_head->head->data) 
    {
        list_get_head(list_head);
        list_head->size --;
        return 0;
    }
    node* n = list_head->head;
    while(n != NULL)
    {
        if(n->data == data)
        {
            node* p = n->pre;
            p->next = n->next;
            if(n->next != NULL) n->next->pre = p;
            free(n);
            list_head->size --;
            return 0;
        }
    }
    return -1;
}

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
            head->next = NULL;
            head->pre = NULL; 
            list_head->size++;
            return 0;
        }
        if(list_head->tail == NULL)
        {
            list_head->tail = list_head->head;
            list_head->tail->pre = head;
            list_head->head = head;
            head->next = list_head->tail;
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
        if(list_head->head == NULL)
        {
            return list_add_head(list_head, data);
        }
        node* tail = (node*)malloc(sizeof(node));
        if(tail == NULL)return -1;
        tail->data = data;
        tail->next = NULL;
        if(list_head->tail == NULL)
        {
            tail->pre = list_head->head; 
            list_head->head->next = tail;
            list_head->tail = tail;
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
        if(head == NULL) return NULL;
        void* data = head->data;
        if(list_head->size == 1)
        {
            list_head->head = NULL;
            list_head->tail = NULL;
            list_head->size --;
            free(head);
            return data; 
        }
        if(list_head->size == 2)
        {
            list_head->head = list_head->tail;
            list_head->head->pre = NULL; 
            list_head->tail = NULL;
            list_head->size --;
            free(head);
            return data;
        }
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
        if(list_head->size < 2)
        {
            return list_get_head(list_head);
        }
        node* tail = list_head->tail;
        void* data = tail->data;
        if(list_head->size == 2)
        {
            list_head->head->next =NULL;
            list_head->tail = NULL;
            list_head->size --;
            free(tail);
            return data;     
        }
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
    if (list_head->size < 3)
    {
        if(list_head->head->data == data) 
        {   
            list_get_head(list_head);
            return 0;
        }
        if(list_head->tail != NULL && list_head->tail->data == data)
        {
            list_get_tail(list_head); 
            return 0;
        }
        else return -1;
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

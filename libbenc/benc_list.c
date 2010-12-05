#include "bencode.h"

benc_list_entry_t * benc_list_entry_new(benc_list_entry_t *next, bobj_t *e);
bobj_t * benc_list_entry_free_return_elem(benc_list_entry_t *l);

void benc_list_free(benc_list_entry_t *head)
{
    benc_list_entry_t *curr = head;
    benc_list_entry_t *tmp;
    while (NULL != curr)
    {
        tmp = curr->next;
        bobj_free(curr->elem);
        free(curr);
        curr = tmp;
    }
}

size_t benc_list_repsize(benc_list_entry_t *head)
{
    size_t repsize = 2;
    benc_list_entry_t *curr = head;
    while (NULL != curr)
    {
        repsize += bobj_repsize(curr->elem);
        curr = curr->next;
    }
    return repsize;
}

void benc_list_encode(bbuf_t *b, benc_list_entry_t *head)
{
    *(b->ptr)++ = 'l';
    benc_list_entry_t *curr = head;
    while (NULL != curr)
    {
        bobj_encode(b, curr->elem);
        curr = curr->next;
    }
    *(b->ptr)++ = 'e';
}

bool benc_list_decode(bbuf_t *b, benc_list_entry_t **head_p)
{
    bbuf_inc_ptr(b);
    benc_list_entry_t **prev_p, *curr;
    prev_p = head_p;
    while (*(b->ptr) != 'e')
    {
        bobj_t *elem = bdec_mem(b);
        if (NULL == elem)
        {
            BENC_LOG_EXCEPTION("couldn't read list element");
            return false;
        }
        curr = benc_list_entry_new(NULL, elem);
        *prev_p = curr;
        prev_p = &(curr->next);
    }
    bbuf_inc_ptr(b);
    return true;
}

benc_list_entry_t * benc_list_entry_new(benc_list_entry_t *next, bobj_t *elem)
{
    benc_list_entry_t *l = (benc_list_entry_t *)malloc(sizeof(benc_list_entry_t));
    l->next = next;
    l->elem = elem;
    return l;
}

bobj_t * benc_list_entry_free_return_elem(benc_list_entry_t *l)
{
    bobj_t *elem = l->elem;
    free(l);
    return elem;
}

bobj_t * bobj_list_new()
{
    bobj_t *obj = bobj_new(BENC_LIST);
    obj->as.list = NULL;
    return obj;
}

void bobj_list_push(bobj_t *obj, bobj_t *elem)
{
    obj->as.list = benc_list_entry_new(obj->as.list, elem);
}

bobj_t * bobj_list_pop(bobj_t *obj)
{
    if (NULL == obj->as.list)
    {
        return NULL;
    }
    
    benc_list_entry_t *new_head = obj->as.list->next;
    bobj_t *elem = benc_list_entry_free_return_elem(obj->as.list);
    obj->as.list = new_head;
    return elem;
}

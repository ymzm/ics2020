/*------------------------------------------------------------------------------
* 版权所有 (C) 2020, ustc howif
*
* 文件名称: list.c
* 文件描述: 双链表接口的实现
* 相关事宜: 无
------------------------------------------------------------------------------*/


#include <list.h>
#include <stdlib.h>

#undef DEBUG

kt_list_t *kt_list_create(void)
{
    register kt_list_t *list = (kt_list_t *) malloc((unsigned) sizeof(kt_list_t));
    if(list != NULL)
    {
        kt_list_init(list);
    }
    return (list);
}
#ifndef SAFETY_OS
int kt_list_delete
(
                kt_list_t *list
)
{
    free((char *) list);
    return OK;
}
#endif

void kt_list_init
(
                register kt_list_t *list
)
{
    list->head = NULL;
    list->tail = NULL;
    return;
}

void kt_list_insert
(
                register kt_list_t *list,
                register kt_list_node_t *prev,
                register kt_list_node_t *node
)
{
    register kt_list_node_t *next;
    #ifdef DEBUG
    if(node == NULL || list == NULL)
    {
        SYSLOG_NORMAL("kt_list_insert: null argument\n");
        return;
    }
    #endif
    if(prev == NULL)
    {
        next = list->head;
        list->head = node;
    }
    else
    {
        next = prev->next;
        prev->next = node;
    }
    if(next == NULL)
    {
        list->tail = node;
    }
    else
    {
        next->prev = node;
    }
    node->next = next;
    node->prev = prev;
}
void kt_list_add
(
                kt_list_t *list,
                kt_list_node_t *node
)
{
    kt_list_insert(list, list->tail, node);
}
void kt_list_remove
(
                kt_list_t *list,
                kt_list_node_t *node
)
{
    #ifdef DEBUG
    kt_list_node_t *ntemp;
    ntemp = KT_LIST_FIRST(list);
    while(ntemp != NULL)
    {
        if(ntemp == node)
        {
            break;
        }
        ntemp = ntemp->next;
    }
    if(ntemp == NULL)
    {
        SYSLOG_NORMAL("kt_list_remove list 0x%x, node 0x%x\n", (int)list, (int)node);
        return;
    }
    #endif
    if((node == NULL) || (list->head == NULL))
    {
        return;
    }
    if(node->prev == NULL)
    {
        list->head = node->next;
    }
    else
    {
        node->prev->next = node->next;
    }
    if(node->next == NULL)
    {
        list->tail = node->prev;
    }
    else
    {
        node->next->prev = node->prev;
    }
    #ifdef DEBUG
    node->next = node->prev = NULL;
    #endif
}

kt_list_node_t *kt_list_get
(
                register kt_list_t *list
)
{
    register kt_list_node_t *node = list->head;
    if(node != NULL)
    {
        list->head = node->next;
        if(node->next == NULL)
        {
            list->tail = NULL;
        }
        else
        {
            node->next->prev = NULL;
        }
    }
    return (node);
}

int kt_list_cnt
(
                kt_list_t *list
)
{
    register kt_list_node_t *node = KT_LIST_FIRST(list);
    register int count = 0;
    while(node != NULL)
    {
        count++;
        node = KT_LIST_NEXT(node);
    }
    return (count);
}
kt_list_node_t *kt_list_foreach
(
                kt_list_t *list,
                int (*routine)(),
                int routine_arg
)
{
    register kt_list_node_t *node = KT_LIST_FIRST(list);
    while((node != NULL) && ((* routine)(node, routine_arg)))
    {
        node = KT_LIST_NEXT(node);
    }
    return (node);
}

#ifndef SAFETY_OS
void kt_list_concat
(
                register kt_list_t *dst_list,
                register kt_list_t *add_list
)
{
    int dstlist_count = kt_list_cnt(dst_list);
    int addlist_count = kt_list_cnt(add_list);
    if(addlist_count == 0)
    {
        return;
    }
    if(dstlist_count == 0)
    {
        *dst_list = *add_list;
    }
    else
    {
        dst_list->tail->next = add_list->head;
        add_list->head->prev = dst_list->tail;
        dst_list->tail = add_list->tail;
    }
    kt_list_init(add_list);
}

void kt_list_extract
(
                register kt_list_t *src_list,
                register kt_list_node_t *start_node,
                register kt_list_node_t *end_node,
                register kt_list_t *dst_list
)
{
    if(start_node->prev == NULL)
    {
        src_list->head = end_node->next;
    }
    else
    {
        start_node->prev->next = end_node->next;
    }
    if(end_node->next == NULL)
    {
        src_list->tail = start_node->prev;
    }
    else
    {
        end_node->next->prev = start_node->prev;
    }
    dst_list->head = start_node;
    dst_list->tail = end_node;
    start_node->prev = NULL;
    end_node->next = NULL;
}
#endif
kt_list_node_t *kt_list_first
(
                kt_list_t *list
)
{
    return (list->head);
}
kt_list_node_t *kt_list_last
(
                kt_list_t *list
)
{
    return (list->tail);
}
kt_list_node_t *kt_list_next
(
                kt_list_node_t *node
)
{
    return (node->next);
}
#ifndef SAFETY_OS
kt_list_node_t *kt_list_get_by_idx
(
                kt_list_t *list,
                int idx
)
{
    register kt_list_node_t *node;
    register int lcnt = kt_list_cnt(list);
    if((idx < 1) || (idx > lcnt))
    {
        return (NULL);
    }
    if(idx < (lcnt >> 1))
    {
        node = list->head;
        while(--idx > 0)
        {
            node = node->next;
        }
    }
    else
    {
        idx -= lcnt;
        node = list->tail;
        while(idx++ < 0)
        {
            node = node->prev;
        }
    }
    return (node);
}
#endif
kt_list_node_t *kt_list_prev
(
                kt_list_node_t *node
)
{
    return (node->prev);
}
#ifndef SAFETY_OS
kt_list_node_t *kt_list_nstep
(
                register kt_list_node_t *node,
                int nStep
)
{
    int i;
    for(i = 0; i < abs(nStep); i++)
    {
        if(nStep < 0)
        {
            node = node->prev;
        }
        else
            if(nStep > 0)
            {
                node = node->next;
            }
        if(node == NULL)
        {
            break;
        }
    }
    return (node);
}
#endif

int kt_list_find
(
                kt_list_t *list,
                register kt_list_node_t *node
)
{
    register kt_list_node_t *next_node;
    register int index = 1;
    next_node = kt_list_first(list);
    while((next_node != NULL) && (next_node != node))
    {
        index++;
        next_node = kt_list_next(next_node);
    }
    if(next_node == NULL)
    {
        return (ERROR);
    }
    else
    {
        return (index);
    }
}
#ifndef SAFETY_OS
static void list_free2
(
                kt_list_t *list,
                void (*free_func)()
)
{
    kt_list_node_t *p1, *p2;
    int lcnt = kt_list_cnt(list);
    if(lcnt > 0)
    {
        p1 = list->head;
        while(p1 != NULL)
        {
            p2 = p1->next;
            free_func((char *)p1);
            p1 = p2;
        }
        list->head = list->tail = NULL;
    }
}
void kt_list_free
(
                kt_list_t *list
)
{
    list_free2(list, free);
}
#endif


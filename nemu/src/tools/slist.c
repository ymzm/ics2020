/*------------------------------------------------------------------------------
* 版权所有 (C) 2020, ustc howif
*
* 文件名称: slist.c
* 文件描述: 双链表接口的实现
* 相关事宜: 无
------------------------------------------------------------------------------*/
//#include <lyre.h>
#include <list.h>
#include <stdlib.h>

int kt_slist_init(kt_slist_t *list)
{
    list->head = NULL;
    list->tail = NULL;
    return (OK);
}

kt_slist_t *kt_slist_create(void)
{
    kt_slist_t *list = (kt_slist_t *) malloc((unsigned) sizeof(kt_slist_t));
    if(list != NULL)
    {
        kt_slist_init(list);
    }
    return (list);
}
#ifndef SAFETY_OS
int kt_slist_delete(kt_slist_t *list)
{
    free((char *) list);
    return OK;
}

void kt_slist_put
(
                kt_slist_t *list,
                kt_slist_node_t *node
)
{
    if((node->next = list->head) == NULL)
    {
        list->head = list->tail = node;
    }
    else
    {
        list->head = node;
    }
}
#endif
void kt_slist_put_tail
(
                kt_slist_t *list,
                kt_slist_node_t *node
)
{
    node->next = NULL;
    if(list->head == NULL)
    {
        list->tail = list->head = node;
    }
    else
    {
        list->tail->next = node;
    }
    list->tail = node;
}
#ifndef SAFETY_OS
kt_slist_node_t *kt_slist_get
(
                register kt_slist_t *list
)
{
    register kt_slist_node_t *node;
    if((node = list->head) != NULL)
    {
        list->head = node->next;
        if(list->tail == node)
        {
            list->tail = NULL;
        }
    }
    return (node);
}

void kt_slist_remove
(
                kt_slist_t *list,
                kt_slist_node_t *del_node,
                kt_slist_node_t *prev_node
)
{
    if(prev_node == NULL)
    {
        list->head = del_node->next;
        if(list->tail == del_node)
        {
            list->tail = NULL;
        }
    }
    else
    {
        prev_node->next = del_node->next;
        if(list->tail == del_node)
        {
            list->tail = prev_node;
        }
    }
}
kt_slist_node_t *kt_slist_prev
(
                kt_slist_t *list,
                kt_slist_node_t *node
)
{
    kt_slist_node_t *tmp_node = list->head;
    if((tmp_node == NULL) || (tmp_node == node))
    {
        return (NULL);
    }
    while(tmp_node->next != NULL)
    {
        if(tmp_node->next == node)
        {
            return (tmp_node);
        }
        tmp_node = tmp_node->next;
    }
    return (NULL);
}

int kt_slist_cnt
(
                kt_slist_t *list
)
{
    register kt_slist_node_t *node = KT_SLIST_FIRST(list);
    register int count = 0;
    while(node != NULL)
    {
        count ++;
        node = KT_SLIST_NEXT(node);
    }
    return (count);
}
#endif
kt_slist_node_t *kt_slist_foreach
(
                kt_slist_t *list,
                int (*routine)(),
                int routine_arg
)
{
    register kt_slist_node_t *node = KT_SLIST_FIRST(list);
    register kt_slist_node_t *pNext;
    while(node != NULL)
    {
        pNext = KT_SLIST_NEXT(node);
        if((* routine)(node, routine_arg) == FALSE)
        {
            break;
        }
        node = pNext;
    }
    return (node);
}

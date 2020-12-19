/*------------------------------------------------------------------------------
* 版权所有 (C) 2020, ustc howif
*
* 文件名称: list.h
* 文件描述: 双链表接口的实现
* 相关事宜: 无
------------------------------------------------------------------------------*/

#ifndef __kt_list_h
#define __kt_list_h

#ifdef __cplusplus
extern "C" {
#endif
/*
* 双链表形式图:
*  _______               ______            ______
* | head--|------------>| next-|--------->| next-|----->NULL
* |       |             |      |          |      |
* |       |     NULL<---| prev |<---------| prev |
* |       |             |      |          |      |
* | tail--|---          | ____ |    ----->| ____ |
* |_______|   \                    /
*              \_________________ /
*
*
* 单链表形式图：
*  _______               ______            ______
* | head--|------------>| next-|--------->| next-|----->NULL
* |       |             |      |          |      |
* |       |             |      |          |      |
* | tail--|---          | ____ |    ----->| ____ |
* |_______|   \                    /
*              \ _________________/
*
*
*/
#define OK 0
#define ERROR -1
#define TRUE 1
#define FALSE 0
/* 单链表和节点的定义 */

typedef struct kt_slist_node_t
{
    struct kt_slist_node_t *next;
} kt_slist_node_t;

typedef struct
{
    kt_slist_node_t *head;
    kt_slist_node_t *tail;
} kt_slist_t;

/* 符号表文件只需包含上面节点、链表的定义；以下定义与声明不需要包含 */

#ifndef SYMTBL_HEADER_EXCLUDED

/* 双链表和节点的定义 */

typedef struct kt_list_node_t
{
    struct kt_list_node_t *next;
    struct kt_list_node_t *prev;
} kt_list_node_t;

typedef struct
{
    struct kt_list_node_t *head;
    struct kt_list_node_t *tail;
} kt_list_t;

/* 双链表初始化 */

#define KT_LIST_INIT(list) \
    do { \
        ((kt_list_t *)(list))->head = NULL; \
        ((kt_list_t *)(list))->tail = NULL; \
    } while(0)

#define KT_LIST_FIRST(list)    (((kt_list_t *)(list))->head) /* 取双链表第一节点 */
#define KT_LIST_LAST(list)     (((kt_list_t *)(list))->tail) /* 取双链表最后节点 */
#define KT_LIST_NEXT(node)     (((kt_list_node_t *)(node))->next)   /* 取双链表下一个节点 */
#define KT_LIST_PREV(node) (((kt_list_node_t *)(node))->prev)       /* 取双链表上一个节点 */
#define KT_LIST_EMPTY(list)    (((kt_list_t *)list)->head == NULL) /* 双链表是否为空 */

/* 在双链表指定节点的后面添加新节点 */

#define KT_LIST_INSERT(list,prev_node,node) \
    do { \
        kt_list_node_t *temp; \
        if ((prev_node) == NULL) \
        { \
            temp = ((kt_list_t *)(list))->head; \
            ((kt_list_t *)(list))->head = (kt_list_node_t *)(node); \
        } \
        else \
        { \
            temp = ((kt_list_node_t *)(prev_node))->next; \
            ((kt_list_node_t *)(prev_node))->next = (kt_list_node_t *)(node); \
        } \
        if (temp == NULL) \
        { \
            ((kt_list_t *)(list))->tail = (kt_list_node_t *)(node); \
        } \
        else \
        { \
            temp->prev = (kt_list_node_t *)(node); \
        } \
        ((kt_list_node_t *)(node))->next = temp; \
        ((kt_list_node_t *)(node))->prev = (kt_list_node_t *)(prev_node); \
    }while(0)

/* 在双链表的最后添加新节点 */

#define KT_LIST_ADD(list,node) \
    do { \
        kt_list_node_t *list_tail = (list)->tail; \
        KT_LIST_INSERT ((list), list_tail, (node)); \
    } while(0)

/* 删除双链表中指定的节点 */

#define KT_LIST_REMOVE(list,node) \
    do { \
        if (((kt_list_node_t *)(node))->prev == NULL) \
        { \
            ((kt_list_t *)(list))->head = ((kt_list_node_t *)(node))->next; \
        } \
        else \
        { \
            ((kt_list_node_t *)(node))->prev->next = ((kt_list_node_t *)(node))->next; \
        } \
        if (((kt_list_node_t *)(node))->next == NULL) \
        { \
            ((kt_list_t *)(list))->tail = ((kt_list_node_t *)(node))->prev; \
        } \
        else \
        { \
            ((kt_list_node_t *)(node))->next->prev = ((kt_list_node_t *)(node))->prev; \
        } \
    } while(0)

/* 从双链表中取出最前节点 */

#define KT_LIST_GET(list,node) \
    do { \
        (node) = (void *)((kt_list_t *)(list))->head; \
        if ((node) != NULL) \
        { \
            ((kt_list_t *)(list))->head = ((kt_list_node_t *)(node))->next; \
            if (((kt_list_node_t *)(node))->next == NULL) \
            { \
                ((kt_list_t *)(list))->tail = NULL; \
            } \
            else \
            { \
                ((kt_list_node_t *)(node))->next->prev = NULL; \
            } \
        } \
    } while(0)

/* 单链表初始化 */

#define KT_SLIST_INIT(list) \
    do { \
        ((kt_slist_t *)(list))->head = NULL; \
        ((kt_slist_t *)(list))->tail = NULL; \
    } while(0)

/* 在单链表的最前端添加新节点 */

#define KT_SLIST_PUT_AT_HEAD(list,node) \
    do { \
        if ((((kt_slist_node_t *)(node))->next = ((kt_slist_t *)(list))->head) == NULL) \
        { \
            ((kt_slist_t *)(list))->head = ((kt_slist_node_t *)(node)); \
            ((kt_slist_t *)(list))->tail = ((kt_slist_node_t *)(node)); \
        } \
        else \
        { \
            ((kt_slist_t *)(list))->head = (node); \
        } \
    } while(0)

/* 在单链表的最后端添加新节点 */

#define KT_SLIST_PUT_AT_TAIL(list,node) \
    do { \
        ((kt_slist_node_t *)(node))->next = NULL; \
        if (((kt_slist_t *)(list))->head == NULL) \
        { \
            ((kt_slist_t *)(list))->head = (kt_slist_node_t *)(node); \
            ((kt_slist_t *)(list))->tail = (kt_slist_node_t *)(node); \
        } \
        else \
        { \
            ((kt_slist_t *)(list))->tail->next = (kt_slist_node_t *)(node); \
        } \
        ((kt_slist_t *)(list))->tail = (kt_slist_node_t *)(node); \
    } while(0)

/* 从单链表中取出最前节点 */

#define KT_SLIST_GET(list,node) \
    do { \
        if (((node) = (void *)((kt_slist_t *)(list))->head) != NULL) \
        { \
            ((kt_slist_t *)(list))->head = ((kt_slist_node_t *)(node))->next; \
        } \
    } while(0)

/* 从单链表中删除指定节点 */

#define KT_SLIST_REMOVE(list,del_node,prev_node) \
    do { \
        if (((kt_slist_node_t *)(prev_node)) == NULL) \
        { \
            ((kt_slist_t *)(list))->head = ((kt_slist_node_t *)(del_node))->next; \
            if (((kt_slist_t *)(list))->tail == ((kt_slist_node_t *)(del_node))) \
            { \
                ((kt_slist_t *)(list))->tail = NULL; \
            } \
        } \
        else \
        { \
            ((kt_slist_node_t *)(prev_node))->next = ((kt_slist_node_t *)(del_node))->next; \
            if (((kt_slist_t *)(list))->tail == ((kt_slist_node_t *)(del_node))) \
            { \
                ((kt_slist_t *)(list))->tail = ((kt_slist_node_t *)(prev_node)); \
            } \
        } \
    } while(0)

/* 从单链表中取出指定节点的上一个节点 */

#define KT_SLIST_PREV(list,node,prev_node) \
    do { \
        kt_slist_node_t *temp; \
        (prev_node) = NULL; \
        temp = ((kt_slist_t *)(list))->head; \
        if ((temp != NULL) && (temp != (node))) \
        { \
            while (temp->next != NULL) \
            { \
                if (temp->next == (node)) \
                { \
                    (prev_node) = temp; \
                    break; \
                } \
                temp = temp->next; \
            } \
        } \
    } while(0)

#define KT_SLIST_FIRST(list)     (((kt_slist_t *)list)->head) /* 取单链表第一节点 */
#define KT_SLIST_LAST(list)      (((kt_slist_t *)list)->tail) /* 取单链表最后节点 */
#define KT_SLIST_NEXT(node)      (((kt_slist_node_t *)node)->next) /* 取单链表指定节点的下一个节点 */
#define KT_SLIST_EMPTY(list)     (((kt_slist_t *)list)->head == NULL) /* 单链表是否为空 */

/* 双链表操作API接口 */

/* 双链表初始化 */
extern void kt_list_init(kt_list_t *list);

/* 动态创建双链表并初始化 */
extern kt_list_t *kt_list_create(void);

/* 对每个节点执行函数操作 */
extern kt_list_node_t *kt_list_foreach(kt_list_t *list,  int (*routine)(), int routine_arg);

/* 获取双链表的节点总个数 */
extern int kt_list_cnt(kt_list_t *list);

/* 在双链表最后端添加新节点 */
extern void kt_list_add(kt_list_t *list, kt_list_node_t *node);

/* 在双链表指定节点后新加节点 */
extern void kt_list_insert(kt_list_t *list, kt_list_node_t *prev, kt_list_node_t *node);

/* 从双链表中删除指定节点 */
extern void kt_list_remove(kt_list_t *list, kt_list_node_t *node);

/* 取下双链表第一个节点 */
extern kt_list_node_t *kt_list_get(kt_list_t *list);

/* 获取双链表的第一个节点（节点仍在链表中） */
extern kt_list_node_t *kt_list_first(kt_list_t *list);

/* 获取双链表的最后一个节点 （节点仍在链表中）*/
extern kt_list_node_t *kt_list_last(kt_list_t *list);

/* 获取双链表的下一个节点 （节点仍在链表中）*/
extern kt_list_node_t *kt_list_next(kt_list_node_t *node);

/* 获取双链表的上一个节点 （节点仍在链表中）*/
extern kt_list_node_t *kt_list_prev(kt_list_node_t *node);

/* 获取指定节点的顺序号。从1开始个数计数 */
extern int kt_list_find(kt_list_t *list,  kt_list_node_t *node);

#ifndef SAFETY_OS
/* 释放双链表内存。仅仅为list所指向的内存 */
extern int kt_list_delete(kt_list_t *list);

/* 获取指定节点偏移nstep个节点的节点。正数后偏移，负数前偏移 */
extern kt_list_node_t *kt_list_nstep(kt_list_node_t *node, int nstep);

/* 获取编号为idx的节点（从1计数，节点仍在链表中） */
extern kt_list_node_t *kt_list_get_by_idx(kt_list_t *list, int idx);

/* 合并两个双链表。add_list追加到dst_list尾端，且add_list被清空 */
extern void kt_list_concat(kt_list_t *dst_list, kt_list_t *add_list);

/* 从双链表中提取子链表 */
extern void kt_list_extract(kt_list_t *src_list, kt_list_node_t *start_node, kt_list_node_t *end_node,
                            kt_list_t *dst_list);

/* 释放双链表上的每一个节点 */
extern void kt_list_free(kt_list_t *list);
#endif /* SAFETY_OS */

/* 单链表操作API接口 */

/* 单链表初始化 */
extern int kt_slist_init(kt_slist_t *list);

/* 动态创建单链表并初始化 */
extern kt_slist_t *kt_slist_create(void);

/* 对每个节点执行函数操作 */
extern kt_slist_node_t *kt_slist_foreach(kt_slist_t *list, int (*routine)(), int routine_arg);

/* 取下单链表第一个节点 */
extern kt_slist_node_t *kt_slist_get(kt_slist_t *list);

/* 获取单链表的指定节点的上一个节点 （节点仍在链表中）*/
extern kt_slist_node_t *kt_slist_prev(kt_slist_t *list, kt_slist_node_t *node);

/* 获取单链表的节点总个数 */
extern int kt_slist_cnt(kt_slist_t *list);

/* 把指定节点放入单链表的首端 */
extern void kt_slist_put(kt_slist_t *list, kt_slist_node_t *node);

/* 把指定节点放入单链表的尾端 */
extern void kt_slist_put_tail(kt_slist_t *list, kt_slist_node_t *node);

/* 从单链表中删除指定节点 */
extern void kt_slist_remove(kt_slist_t *list, kt_slist_node_t *del_node,
                            kt_slist_node_t *prev_node);
#ifndef SAFETY_OS
/* 释放单链表内存。仅仅为list所指向的内存 */
extern int kt_slist_delete(kt_slist_t *list);
#endif

#endif /* SYMTBL_HEADER_EXCLUDED */

#ifdef __cplusplus
}
#endif

#endif /* __kt_list_h */
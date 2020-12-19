#include "watchpoint.h"
#include <list.h>
#include "expr.h"
#include <stdlib.h>
#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool()
{
    int i;
    for(i = 0; i < NR_WP; i ++)
    {
        wp_pool[i].NO = i;
        wp_pool[i].next = &wp_pool[i + 1];
    }
    wp_pool[NR_WP - 1].next = NULL;
    head = NULL;
    free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

//pa1.3
static int NO = 32;
//从free_中返回一个空闲wp结构
WP *wp_alloc()
{
    WP *p;
    if(!free_)
    {
        printf("free linkedlist is empty\n");
        p = malloc(sizeof(struct watchpoint));
        assert(p);
        p->NO = NO;
        //p->next = NULL;
        NO++;
    }
    else
    {
        p = free_;
        free_ =  free_->next;
    }
    p->next = head;
    head = p;
    return head;
}

void free_wp(WP *wp)
{
    if(!head)
    {
        printf("There is no wp to free\n");
        return;
    }
    if(wp == head)
    {
        WP *p = head;
        head = head->next;
        p->next = free_;
        free_ = free_->next;
        return ;
    }
    WP *p = head;
    while(p != NULL && p->next != wp)
    {
        p = p->next;
    }
    assert(p != NULL);
    p->next = wp->next;
    wp->next = free_;
    free_ = wp;
}

bool check_wp()
{
    DEBUGP();
    bool changed = false;
    WP *p = head;
    for(; p != NULL; p = p->next)
    {
        bool success = true;
        word_t val = expr(p->expr_str, &success);
        Assert(success, "expression compute error\n");
        if(val != p->value)
        {
            changed = true;
            printf("watchpoint NO.%d , %s\n", p->NO, p->expr_str);
            printf("Old value: %d\n", p->value);
            printf("New value: %d\n", val);
            Log(" ");  //for what?
            p->value = val;
            p->hit++;
        }
    }
    return changed;
}

bool del_wp(int n)
{
    WP *p = head;
    bool is_free = false;
    while(p)
    {
        if(p->NO == n)
        {
            free_wp(p);
            is_free = true;
            break;
        }
        p = p->next;
    }
    return is_free;
}

void print_wp()
{
    if(head == NULL)
    {
        printf("There is no watchpoints!\n");
        return;
    }
    printf("Num     What     Value\n");
    WP *p = head;
    while(p != NULL)
    {
        printf("%-8d%-9s%u(%#x)\n", p -> NO, p -> expr_str, p->value, p->value);
        if(p -> hit > 0) { printf("        breakpoint already hit %d time\n", p->hit); }
        p = p -> next;
    }
}
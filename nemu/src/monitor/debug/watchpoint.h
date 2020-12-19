#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint
{
    int NO;
    struct watchpoint *next;
    /* TODO: Add more members if necessary */
    char expr_str[256];
    uint32_t value;
    uint32_t hit;
} WP;

extern WP *wp_alloc();
extern void free_wp(WP *wp);

extern bool check_wp();

extern bool del_wp(int n);

extern void print_wp();

#endif

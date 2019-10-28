#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#include "isa/watchpoint.h"
#include "monitor/expr.h"

typedef struct watchpoint {
  int NO;                           // 编号
  struct node *val;                 // 当前的值
  bool access_mem;                  // 是否访问内存
  char *expression;                 // 表达式，当为NULL时代表该wp没有使用
  struct list_node *reg_list;       // 表达式中包含的寄存器链表
  struct list_node *mem_list;       // 表达式中包含的当前内存链表
  struct list_node *mem_expr_list;  // 表达式中包含的访存表达式链表
  struct watchpoint *prev;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

static inline void init_wp(WP *wp) {
  wp->val = NULL;
  wp->access_mem = false;
  wp->expression = NULL;
  wp->mem_expr_list = NULL;
  wp->mem_list = NULL;
  wp->reg_list = NULL;
}

void add_wp(char *expr);
void del_wp(int NO);
void wp_display();
bool wp_detect(vaddr_t pc);

extern bool wp_reg_changed;
extern bool wp_access_mem;
extern bool wp_reg_flag;

#endif

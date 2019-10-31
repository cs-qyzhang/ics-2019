/* 调试器中表达式解析定义 */
#ifndef __EXPR_H__
#define __EXPR_H__

#include "common.h"

// 保存表达式结果
struct node {
  enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL
  } type;

  union {
    int32_t  type_int;
    float    type_float;
    int32_t   type_bool;
  };
};

struct expr_info {
  bool is_const;
  bool access_mem;
  struct list_node *reg_list;
  struct list_node *mem_list;
  struct list_node *mem_expr_list;
};

static inline void init_expr_info(struct expr_info *info) {
  if (info != NULL) {
    info->is_const = true;
    info->access_mem = false;
    info->reg_list = NULL;
    info->mem_list = NULL;
    info->mem_expr_list = NULL;
  }
}

static inline bool node_cmp(struct node *a, struct node *b) {
  assert(a != NULL && b != NULL);
  return (a->type == b->type) && (a->type_int == b->type_int);
}

static inline void print_node(struct node *val) {
  if (val->type == TYPE_INT) {
    printf("%d", val->type_int);
  } else if (val->type == TYPE_FLOAT) {
    printf("%f", val->type_float);
  } else {
    printf("%s", val->type_bool ? "true" : "false");
  }
}

struct node *expr(char *, bool *, struct expr_info *);

#endif

#ifndef _NEMU_EXPR_DEF_H_
#define _NEMU_EXPR_DEF_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "monitor/expr.h"
#include "isa/reg.h"
#include "rtl/rtl.h"

extern struct expr_info *info;

enum relop{
  LT,
  GT,
  LE,
  GE,
  EQ,
  NE
};

static inline void expr_error(char *msg) {
  Log("%s", msg);
}

static inline struct node *mknode() {
  return (struct node *)malloc(sizeof(struct node));
}

static inline struct node *expr_access_memory(struct node *op) {
  if (info) {
    info->is_const = false;
    info->access_mem = true;
  }
  if (op->type == TYPE_BOOL || op->type == TYPE_FLOAT) {
    expr_error("memory address isn't integer!");
  } else {
    op->type = TYPE_INT;
    rtlreg_t tmp;
    rtl_lm(&tmp, (vaddr_t *)&op->type_int, 4);
    op->type_int = tmp;
  }
  return op;
}

static inline struct node *expr_access_register(char *reg) {
  int index = isa_get_reg_index(reg);
  if (info) {
    info->is_const = false;
    int wp_reg_index = (index < 16) ? index % 8 : index % 4;
    if (info->reg_list == NULL) {
      info->reg_list = (struct list_node *)malloc(sizeof(struct list_node));
      init_list(info->reg_list, wp_reg_index, NULL);
    } else {
      list_add(info->reg_list, wp_reg_index, NULL);
    }
  }
  struct node *ret = mknode();
  ret->type = TYPE_INT;
  ret->type_int = isa_access_reg(index);
  return ret;
}

static inline struct node *expr_reverse(struct node *op) {
  if (op->type == TYPE_BOOL) {
    expr_error("boolean!");
  } else if (op->type == TYPE_INT) {
    op->type_int = -op->type_int;
  } else {
    op->type_float = -op->type_float;
  }
  return op;
}

#define expr_helper_func(op_name, op) \
  static inline struct node *expr_##op_name (struct node *op1, struct node *op2) {  \
    if (op1->type == TYPE_BOOL || op2->type == TYPE_BOOL) { \
      expr_error("boolean!"); \
    } else if (op1->type == TYPE_INT && op2->type == TYPE_FLOAT) {  \
      op1->type = TYPE_FLOAT; \
      op1->type_float = op1->type_int op op2->type_float; \
    } else if (op1->type == TYPE_FLOAT && op2->type == TYPE_INT) {  \
      op1->type_float = op1->type_float op op2->type_float; \
    } else if (op1->type == TYPE_INT && op2->type == TYPE_INT){ \
      if (#op[0] == '/') { /* 这里整数除法结果为浮点数 */ \
        op1->type = TYPE_FLOAT; \
        op1->type_float = (float)op1->type_int / (float)op2->type_int;  \
      } else {  \
        op1->type_int = op1->type_int op op2->type_int;  \
      } \
    } else {  \
      op1->type_float = op1->type_float op op2->type_float; \
    } \
    free(op2);  \
    return op1; \
  }

expr_helper_func(add, +)
expr_helper_func(sub, -)
expr_helper_func(mul, *)
expr_helper_func(div, /)

static inline struct node *expr_addi(struct node *op1, int op2) {
  if (op1->type == TYPE_BOOL) {
    expr_error("boolean!");
  } else if (op1->type == TYPE_INT) {
    op1->type_int += op2;
  } else {
    op1->type_float += op2;
  }
  return op1;
}

#define expr_bitwise_helper_func(op_name, op) \
  static inline struct node *expr_##op_name (struct node *op1, struct node *op2) {  \
    if (op1->type == TYPE_INT && op2->type == TYPE_INT) { \
      op1->type_int = op1->type_int op op2->type_int;  \
    } else {  \
      expr_error(#op_name); \
    } \
    free(op2);  \
    return op1; \
  }

expr_bitwise_helper_func(lshift, <<)
expr_bitwise_helper_func(rshift, <<)
expr_bitwise_helper_func(bitand, &)
expr_bitwise_helper_func(bitor, |)
expr_bitwise_helper_func(bitnor, ^)
expr_bitwise_helper_func(mod, %)  // 取模操作需要的类型与位运算相同

#define expr_comp(op1, op2, relop) ({  \
    int32_t result = 0; \
    switch (relop) {  \
      case LT: result = op1 < op2; break; \
      case GT: result = op1 > op2; break; \
      case LE: result = op1 <= op2; break;  \
      case GE: result = op1 >= op2; break;  \
      case EQ: result = op1 == op2; break;  \
      case NE: result = op1 != op2; break;  \
    } \
    result; \
  })

static inline struct node *expr_relation(struct node *op1, struct node *op2, enum relop rel) {
  if (op1->type == TYPE_BOOL || op2->type == TYPE_BOOL) {
    expr_error("boolean");
    return NULL;
  } else if (op1->type == TYPE_INT && op2->type == TYPE_FLOAT) {
    op1->type_bool = expr_comp(op1->type_int, op2->type_float, rel);
  } else if (op1->type == TYPE_FLOAT && op2->type == TYPE_INT) {
    op1->type_bool = expr_comp(op1->type_float, op2->type_int, rel);
  } else if (op1->type == TYPE_INT && op2->type == TYPE_INT) {
    op1->type_bool = expr_comp(op1->type_int, op2->type_int, rel);
  } else {
    op1->type_bool = expr_comp(op1->type_float, op2->type_float, rel);
  }
  op1->type = TYPE_BOOL;
  free(op2);
  return op1;
}

#define expr_logical_helper_func(op_name, op) \
  static inline struct node *expr_##op_name (struct node *op1, struct node *op2) {  \
    if (op1->type == TYPE_INT && op2->type == TYPE_FLOAT) { \
      op1->type_bool = op1->type_int op op2->type_float;  \
    } else if (op1->type == TYPE_FLOAT && op2->type == TYPE_INT) {  \
      op1->type_bool = op1->type_float op op2->type_int;  \
    } else if (op1->type == TYPE_FLOAT && op2->type == TYPE_FLOAT) {  \
      op1->type_bool = op1->type_float op op2->type_float;  \
    } else {  /* 当两个为int或bool时可以直接比较 */ \
      op1->type_bool = op1->type_int op op2->type_int;  \
    } \
    op1->type = TYPE_BOOL;  \
    free(op2);  \
    return op1; \
  }

expr_logical_helper_func(and, &&)
expr_logical_helper_func(or, ||)

static inline struct node *expr_cond(struct node *cond, struct node *true_val, struct node *false_val) {
  return cond->type_bool ? true_val : false_val;
}

#endif // _NEMU_EXPR_DEF_H_

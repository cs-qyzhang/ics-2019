#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "cpu/exec.h"

#define NR_WP 32

// 事实上为WP_REG_FLAG类型
bool wp_reg_flag, wp_reg_changed, wp_access_mem;
bool wp_eip_flag;
uint32_t wp_eip_watch;
struct watchpoint *wp_eip;

// 表示寄存器被哪一个watchpoint所监视
struct list_node wp_reg[8];

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;  // 双链表

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    init_wp(&wp_pool[i]);
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[(i + 1) % NR_WP];
    wp_pool[i].prev = &wp_pool[(i + NR_WP - 1) % NR_WP];
  }

  head = NULL;
  free_ = &wp_pool[0];

  for (i = 0; i < 8; ++i) {
    init_list(&wp_reg[i], -1, NULL);
  }
}

/* TODO: Implement the functionality of watchpoint */
void add_wp(char *wp_expr) {
  assert(wp_expr != NULL);
  if (free_ == NULL) {
    Log("no free watchpoint available!");
    return;
  }

  bool success;
  struct expr_info info;
  struct node *expr_val;
  init_expr_info(&info);
  expr_val = expr(wp_expr, &success, &info);
  if (!success) {
    Log("WRONG EXPR!");
    return;
  }
  if (info.is_const) {
    Log("watchpoint expr is const!");
    return;
  }

  // 修改free_链表，取出第一个
  struct watchpoint *cur = free_;
  if (free_->next == free_) {
    free_ = NULL;
  } else {
    free_->prev->next = free_->next;
    free_->next->prev = free_->prev;
    free_ = free_->next;
  }

  // 修改head链表，插入链表的最后
  if (head == NULL) {
    cur->next = cur;
    cur->prev = cur;
    head = cur;
  } else {
    cur->next = head;
    cur->prev = head->prev;
    head->prev->next = cur;
    head->prev = cur;
  }

  cur->val = expr_val;
  cur->expression = wp_expr;
  cur->reg_list = info.reg_list;
  cur->access_mem = info.access_mem;

  if (info.reg_list->data == 8) {
    char *p = wp_expr;
    while (*p != '=')
      p++;
    p++;
    p++;
    wp_eip_flag = true;
    wp_eip_watch = expr(p, &success, NULL)->type_int;
    wp_eip = cur;
  } else {
    // 维护wp_reg_flag等信息
    struct list_node *p;
    p = cur->reg_list;
    while (p) {
      int reg = p->data;
      wp_reg_flag |= reg_mask[reg];
      list_add(&wp_reg[reg], cur->NO, NULL);
      p = p->next == cur->reg_list ? NULL : p->next;
    }
  }

  Log("add watch point %d", cur->NO);
}

void del_wp(int NO) {
  if (NO < 0 || NO >= NR_WP || wp_pool[NO].expression == NULL) {
    Log("watchpoint %d doesn't exist!", NO);
    return;
  }

  // 修改head链表
  if (wp_pool[NO].next == &wp_pool[NO]) {
    head = NULL;
  } else {
    wp_pool[NO].next->prev = wp_pool[NO].prev;
    wp_pool[NO].prev->next = wp_pool[NO].next;
  }

  // 修改free_链表，插入链表的最后
  if (free_ == NULL) {
    free_ = &wp_pool[NO];
    free_->next = free_;
    free_->prev = free_;
  } else {
    wp_pool[NO].next = free_;
    wp_pool[NO].prev = free_->prev;
    free_->prev->next = &wp_pool[NO];
    free_->prev = &wp_pool[NO];
  }

  // 维护wp_reg_flag等信息
  struct list_node *p, *q;
  p = wp_pool[NO].reg_list;
  while (p) {
    int reg = p->data;
    list_del(&wp_reg[reg], NO, NULL);
    if (wp_reg[reg].next == &wp_reg[reg])
      wp_reg_flag ^= reg_mask[reg];  // 异或将该标志位取消
    q = p;
    p = p->next == wp_pool[NO].reg_list ? NULL : p->next;
    free(q);
  }

  free(wp_pool[NO].expression);
  wp_pool[NO].expression = NULL;
  Log("delete watchpoint %d", NO);
}

void wp_display() {
  printf("NO  expression\n");
  WP *p = head;
  while (p) {
    printf("%-4d%s\n", p->NO, p->expression);
    p = p->next == head ? NULL : p->next;
  }
}

bool expr_detect(WP *wp, vaddr_t pc) {
  struct node *new_val;
  bool success;
  new_val = expr(wp->expression, &success, NULL);
  assert(success == true);
  if (!node_cmp(new_val, wp->val)) { // 监视点值改变
    putchar('\n');
    printf("Watchpoint %d changed at address 0x%x! Watchpoint expression: %s\n", wp->NO, pc, wp->expression);
    printf("Old value: ");
    print_node(wp->val);
    putchar('\n');
    printf("New value: ");
    print_node(new_val);
    putchar('\n');
    putchar('\n');
    free(wp->val);
    wp->val = new_val;
    return true;
  }
  return false;
}

bool wp_detect(vaddr_t pc) {
  int8_t reg_changed;
  bool wp_changed = false;
  if (head == NULL)
    return false;
  if (wp_access_mem) {
    WP *p = head;
    while (p) {
      if (p->access_mem)
        wp_changed = wp_changed || expr_detect(p, pc);
      p = p->next == head ? NULL : p->next;
    }
  } else if ((reg_changed = wp_reg_flag & wp_reg_changed)) {  // 存在监视的寄存器被修改
    for (int i = 0; i < 8 && reg_changed; ++i, reg_changed = reg_changed >> 1) {
      if (reg_changed & 0x01) {
        struct list_node *p = wp_reg[i].next;
        while (p != &wp_reg[i]) {
          wp_changed = wp_changed || expr_detect(&wp_pool[p->data], pc);
          p = p->next;
        }
      } //if
    } // for
  } // if
  return wp_changed;
}

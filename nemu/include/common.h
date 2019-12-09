#ifndef __COMMON_H__
#define __COMMON_H__

#define DEBUG
// #define CALL_LOG
// #define DIFF_TEST

#if _SHARE
// do not enable these features while building a reference design
#undef DIFF_TEST
#undef DEBUG
#endif

/* You will define this macro in PA2 */
#define HAS_IOE

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

typedef uint8_t bool;

typedef uint32_t rtlreg_t;

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

typedef uint16_t ioaddr_t;

#define false 0
#define true 1

#include "debug.h"
#include "macro.h"

struct list_node {
  union {
    void *ptr;
    int64_t data;
  };
  struct list_node *prev;
  struct list_node *next;
};

static inline void init_list(struct list_node * l, int64_t val, void *ptr) {
  if (ptr)
    l->ptr = ptr;
  else 
    l->data = val;
  l->prev = l;
  l->next = l;
}

static inline void list_add(struct list_node * l, int64_t data, void *ptr) {
  struct list_node *new_node = (struct list_node *)malloc(sizeof(struct list_node));
  if (ptr) {
    new_node->ptr = ptr;
  } else {
    new_node->data = data;
  }
  new_node->prev = l->prev;
  new_node->next = l;
  l->prev->next = new_node;
  l->prev = new_node;
}

static inline void list_del(struct list_node *l, int64_t data, void *ptr) {
  struct list_node *p = l->next;
  if (ptr) {
    while (p != l) {
      if (p->ptr == ptr) {
        p->prev->next = p->next;
        p->next->prev = p->prev;
        return;
      }
      p = p->next;
    }
  } else {
    while (p != l) {
      if (p->data == data) {
        p->prev->next = p->next;
        p->next->prev = p->prev;
        return;
      }
      p = p->next;
    }
  }
}

#endif

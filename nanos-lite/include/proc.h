#ifndef __PROC_H__
#define __PROC_H__

#include "common.h"
#include "memory.h"

#define STACK_SIZE (8 * PGSIZE)
#define MAX_NR_PROC 4

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    _Context *cp;
    _AddressSpace as;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
} PCB;

extern PCB *current;
extern PCB *fg_pcb;
extern PCB pcb[MAX_NR_PROC];
extern void naive_uload(PCB *pcb, const char *filename);
extern void context_uload(PCB *pcb, const char *filename);
extern void run_proc(PCB *pcb);

#endif

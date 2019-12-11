#include "proc.h"

PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
PCB *fg_pcb = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void run_proc(PCB *pcb) {
  extern void _urun(_AddressSpace *as, void (*entry)());
  _urun(&pcb->as, (void (*)())pcb->cp->eip);
}

void init_proc() {
  extern void context_kload(PCB *pcb, void *entry);
  extern void context_uload(PCB *pcb, const char *filename);

  //context_kload(&pcb[0], (void *)hello_fun);

  //switch_boot_pcb();
  current = &pcb[0];
  fg_pcb = &pcb[1];

  Log("Initializing processes...");

  // load program here
  //naive_uload(NULL, "/bin/dummy");
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");
  run_proc(&pcb[0]);
}

_Context* schedule(_Context *prev) {
  static uint32_t cnt = 0;
  const int fg_nice = 2000;

  current->cp = prev;
  current = (cnt++ % fg_nice) ? fg_pcb : &pcb[0];

  return current->cp;
}

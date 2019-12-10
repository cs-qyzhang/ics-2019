#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  extern _Context* do_syscall(_Context *c);
  extern _Context* schedule(_Context *prev);

  switch (e.event) {
    case _EVENT_YIELD:
      printf("yield!\n");
      return schedule(c);

    case _EVENT_SYSCALL:
      return do_syscall(c);

    default:
      panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}

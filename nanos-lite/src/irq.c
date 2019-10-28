#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  extern _Context* do_syscall(_Context *c);

  switch (e.event) {
    case _EVENT_YIELD:
      printf("yield!\n");
      break;

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

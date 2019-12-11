#include "common.h"
#include "am.h"

static _Context* do_event(_Event e, _Context* c) {
  extern _Context* do_syscall(_Context *c);
  extern _Context* schedule(_Context *prev);

  switch (e.event) {
    case _EVENT_IRQ_TIMER:  // 接收到时钟中断时按照yield处理
#ifdef DEBUG
      Log("EVENT_IRQ_TIMER");
#endif
    case _EVENT_YIELD:
#ifdef DEBUG
      Log("yield!");
#endif
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

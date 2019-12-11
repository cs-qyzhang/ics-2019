#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
#ifdef DEBUG
  Log("mm_brk: %x, max_brk: %x\n", (uint32_t)new_brk, (uint32_t)current->max_brk);
#endif
  // 首次调用记录max_brk
  if (current->max_brk == 0) {
    current->max_brk = (new_brk & 0xfff) ? ((new_brk & ~0xfff) + PGSIZE) : new_brk;
    return 0;
  }

  for (; current->max_brk < new_brk; current->max_brk += PGSIZE) {
    _map(&current->as, (void *)current->max_brk, new_page(1), 0);
  }

  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %x", (uint32_t)pf);
  Log("_heap.start: %x, _heap.end: %x", (uint32_t)_heap.start, (uint32_t)_heap.end);

  _vme_init(new_page, free_page);
}

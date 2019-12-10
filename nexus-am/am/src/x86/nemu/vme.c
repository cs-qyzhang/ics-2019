#include <am.h>
#include <x86.h>
#include <klib.h>
#include <nemu.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE},
  {.start = (void*)MMIO_BASE,  .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
  printf("enable vme!\n");
  vme_enable = 1;

  return 0;
}

int _protect(_AddressSpace *as) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable) {
    set_cr3(c->as->ptr);
    cur_as = c->as;
  }
}

// 将va虚拟地址映射到pa物理地址，将该映射关系写入页表中
int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  PTE *pdir = as->ptr;
  PDE *pptab = &pdir[PDX(va)];

  if (!(*pptab & PTE_P)) {  // 如果页表不存在则分配一个页目录
    *pptab = (uint32_t)pgalloc_usr(1);
    memset((void *)*pptab, 0, PGSIZE);
    *pptab = *pptab | PTE_P;
  }

  PDE *ptab = &(((PDE *)PTE_ADDR(*pptab))[PTX(va)]);
  if (*ptab & PTE_P) {  // 如果页已经存在则报错
    printf("ERROR:vme _map(): page map already exists!");
    assert(0); 
  }
  *ptab = PTE_ADDR(pa) | PTE_P;

  return 0;
}

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  _Context *context = ustack.end - sizeof(_Context) - 0x20;
  memset(context, 0x00, sizeof(_Context) + 0x20);
  context->cs = 8;
  context->eip = (uint32_t)entry;
  memset(&context->eflags, 0x02, sizeof(context->eflags));
  context->as = NULL;

  return context;
}

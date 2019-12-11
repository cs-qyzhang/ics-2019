#ifndef __ARCH_H__
#define __ARCH_H__

#include <stdint.h>

// XXX
struct _Context {
  struct _AddressSpace *as;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t irq;
  uint32_t eip, cs;
  union{
    struct {
      uint32_t CF   : 1;
      uint32_t      : 1;
      uint32_t PF   : 1;
      uint32_t      : 1;
      uint32_t AF   : 1;
      uint32_t      : 1;
      uint32_t ZF   : 1;
      uint32_t SF   : 1;
      uint32_t TF   : 1;
      uint32_t IF   : 1;
      uint32_t DF   : 1;
      uint32_t OF   : 1;
      uint32_t IOPL : 2;
      uint32_t NT   : 1;
      uint32_t      : 1;
      uint32_t RF   : 1;
      uint32_t VM   : 1;
      uint32_t      : 14;
    };
    uint32_t val;
  } eflags;
};

// XXX https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux
#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPR5 esi
#define GPR6 edi
#define GPR7 ebp
#define GPRx eax

#endif

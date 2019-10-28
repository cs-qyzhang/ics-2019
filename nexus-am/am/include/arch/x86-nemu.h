#ifndef __ARCH_H__
#define __ARCH_H__

#include <stdint.h>

// XXX
struct _Context {
  struct _AddressSpace *as;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t irq;
  uint32_t eip, cs, eflags;
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

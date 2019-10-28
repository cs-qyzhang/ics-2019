#include "isa/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // XXX: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:  *dest = cpu.eflags.OF; break;
    case CC_B:  *dest = cpu.eflags.CF; break;
    case CC_E:  *dest = cpu.eflags.ZF; break;
    case CC_BE: *dest = cpu.eflags.CF | cpu.eflags.ZF; break;
    case CC_S:  *dest = cpu.eflags.SF; break;
    case CC_L:  *dest = cpu.eflags.SF ^ cpu.eflags.OF; break;
    case CC_LE: *dest = cpu.eflags.ZF | (cpu.eflags.SF ^ cpu.eflags.OF); break;
    case CC_P: panic("n86 does not have PF"); break;
    default: panic("should not reach here"); break;
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
  // printf("setcc: %d\n", *dest);
}

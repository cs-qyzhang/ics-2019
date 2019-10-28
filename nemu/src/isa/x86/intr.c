#include "rtl/rtl.h"
#include "isa/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* XXX: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  uint32_t lo, hi, tmp;

  assert((NO + 1) * 8 <= cpu.IDTR.limit);
  rtl_push((rtlreg_t *)&cpu.eflags);
  rtl_li(&tmp, cpu.CS);
  rtl_push(&tmp);
  rtl_push(&ret_addr);

  lo = vaddr_read(cpu.IDTR.base + NO * 8, 4);
  hi = vaddr_read(cpu.IDTR.base + NO * 8 + 4, 4);
  rtl_j((hi & 0xffff0000u) | (lo & 0x0ffffu));

#ifdef CALL_LOG
  static size_t call_cnt = 0;
  extern FILE *call_fp;
  extern void reopen_call_log();
  ++call_cnt;
  if (call_cnt == 1024 * 1024) {
    reopen_call_log();
    call_cnt = 0;
  }
  fprintf(call_fp, "int  %x -> %x\n", ret_addr, (hi & 0xffff0000u) | (lo & 0x0ffffu));
  fflush(call_fp);
#endif
}

bool isa_query_intr(void) {
  return false;
}

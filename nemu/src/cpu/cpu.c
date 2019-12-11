#include "cpu/exec.h"
#include "monitor/watchpoint.h"

CPU_state cpu;

rtlreg_t s0, s1, t0, t1, ir;

/* shared by all helper functions */
DecodeInfo decinfo;

void decinfo_set_jmp(bool is_jmp) {
  decinfo.is_jmp = is_jmp;
}

void isa_exec(vaddr_t *pc);

vaddr_t exec_once(void) {
  wp_access_mem = false;
  wp_reg_changed = 0;

  decinfo.seq_pc = cpu.pc;
  isa_exec(&decinfo.seq_pc);
  update_pc();

  // 中断检测
  extern bool isa_query_intr(void);
  if (isa_query_intr()) update_pc();

  return decinfo.seq_pc;
}

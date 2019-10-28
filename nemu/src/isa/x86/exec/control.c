#include "cpu/exec.h"
#include "cc.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
#ifdef CALL_LOG
  static size_t call_cnt = 0;
  extern FILE *call_fp;
  extern void reopen_call_log();
  ++call_cnt;
  if (call_cnt == 1024 * 1024) {
    reopen_call_log();
    call_cnt = 0;
  }
  fprintf(call_fp, "jmp  %x -> %x\n", *pc, decinfo.jmp_pc);
  fflush(call_fp);
#endif
  rtl_j(decinfo.jmp_pc);

  print_asm("jmp %x", decinfo.jmp_pc);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = decinfo.opcode & 0xf;
  rtl_setcc(&s0, cc);
  rtl_li(&s1, 0);
  rtl_jrelop(RELOP_NE, &s0, &s1, decinfo.jmp_pc);

  print_asm("j%s %x", get_cc_name(cc), decinfo.jmp_pc);
}

make_EHelper(jmp_rm) {
  rtl_jr(&id_dest->val);

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  // XXX();
  
#ifdef CALL_LOG
  static size_t call_cnt = 0;
  extern FILE *call_fp;
  extern void reopen_call_log();
  ++call_cnt;
  if (call_cnt == 1024 * 1024) {
    reopen_call_log();
    call_cnt = 0;
  }
  fprintf(call_fp, "call %x -> %x\n", *pc, *pc + id_dest->val);
  fflush(call_fp);
#endif

  rtl_push(pc);
  rtl_j(*pc + id_dest->val);

  print_asm("call %x", decinfo.jmp_pc);
}

make_EHelper(ret) {
  // XXX();
  rtl_pop(&s0);

#ifdef CALL_LOG
  static size_t call_cnt = 0;
  extern FILE *call_fp;
  extern void reopen_call_log();
  ++call_cnt;
  if (call_cnt == 1024 * 1024) {
    reopen_call_log();
    call_cnt = 0;
  }
  fprintf(call_fp, "ret  %x -> %x\n", *pc, s0);
  fflush(call_fp);
#endif

  rtl_j(s0);

  print_asm("ret");
}

make_EHelper(ret_imm) {
  // XXX();
  rtl_pop(&s0);
  rtl_j(s0);
  reg_l(R_ESP) = reg_l(R_ESP) + id_dest->val;

  print_asm("ret %s", id_dest->str);
}

make_EHelper(call_rm) {
  // XXX();

#ifdef CALL_LOG
  static size_t call_cnt = 0;
  extern FILE *call_fp;
  extern void reopen_call_log();
  ++call_cnt;
  if (call_cnt == 1024 * 1024) {
    reopen_call_log();
    call_cnt = 0;
  }
  fprintf(call_fp, "call %x -> %x\n", *pc, id_dest->val);
  fflush(call_fp);
#endif

  rtl_push(pc);
  rtl_jr(&id_dest->val);

  print_asm("call *%s", id_dest->str);
}

#include "cpu/exec.h"

make_EHelper(lidt) {
  // XXX();
  cpu.IDTR.limit = vaddr_read(id_dest->addr, 2);
  cpu.IDTR.base = vaddr_read(id_dest->addr + 2, 4);

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  // XXX();
  extern void raise_intr(uint32_t NO, vaddr_t ret_addr);
  raise_intr(id_dest->val, decinfo.seq_pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
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
  fprintf(call_fp, "iret %x -> %x\n", *pc, s0);
  fflush(call_fp);
#endif

  rtl_j(s0);
  rtl_pop(&s0);
  cpu.CS = s0;
  rtl_pop(&s0);
  rtl_li((void *)&cpu.eflags, s0);

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  // XXX();
  switch (decinfo.opcode & 0xffu) {
    case 0xe4:  // in al, imm8
      s0 = pio_read_b(id_dest->val);
      rtl_sr(R_AL, &s0, 1);
      break;
    case 0xe5:  // in ax/eax, imm8
      if (decinfo.isa.is_operand_size_16) { // in ax, imm8
        s0 = pio_read_w(id_dest->val);
        rtl_sr(R_AX, &s0, 2);
      } else {  // in eax, imm8
        s0 = pio_read_l(id_dest->val);
        rtl_sr(R_EAX, &s0, 4);
      }
      break;
    case 0xec:  // in al, dx
      rtl_lr(&s0, R_DX, 2);
      s1 = pio_read_b(s0);
      rtl_sr(R_AL, &s1, 1);
      break;
    case 0xed:  // in ax/eax, dx
      rtl_lr(&s0, R_DX, 2);
      if (decinfo.isa.is_operand_size_16) {
        s1 = pio_read_w(s0);
        rtl_sr(R_AX, &s1, 2);
      } else {
        s1 = pio_read_l(s0);
        rtl_sr(R_EAX, &s1, 4);
      }
      break;
    default:
      assert(0);
  }

  print_asm_template2(in);
}

make_EHelper(out) {
  // XXX();
  switch (decinfo.opcode & 0xffu) {
    case 0xe6:  // out imm8, al
      rtl_lr(&s0, R_AL, 1);
      pio_write_b(id_dest->val, s0);
      break;
    case 0xe7:  // out imm8, ax/eax
      if (decinfo.isa.is_operand_size_16) {
        rtl_lr(&s0, R_AX, 2);
        pio_write_w(id_dest->val, s0);
      } else {
        rtl_lr(&s0, R_EAX, 4);
        pio_write_l(id_dest->val, s0);
      }
      break;
    case 0xee:  // out dx, al
      rtl_lr(&s0, R_DX, 2);
      rtl_lr(&s1, R_AL, 1);
      pio_write_b(s0, s1);
      break;
    case 0xef:  // out dx, ax/eax
      rtl_lr(&s0, R_DX, 2);
      if (decinfo.isa.is_operand_size_16) {
        rtl_lr(&s1, R_AX, 2);
        pio_write_w(s0, s1);
      } else {
        rtl_lr(&s1, R_EAX, 4);
        pio_write_l(s0, s1);
      }
      break;
    default:
      assert(0);
  }

  print_asm_template2(out);
}

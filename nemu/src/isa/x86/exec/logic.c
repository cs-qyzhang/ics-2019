#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  // XXX();
  rtl_and(&s1, &id_dest->val, &id_src->val);
  rtl_li(&s0, 0);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  rtl_update_ZFSF(&s1, id_dest->width);

  // printf("test: %x = %x & %x\n", s1, id_dest->val, id_src->val);
  print_asm_template2(test);
}

make_EHelper(and) {
  // XXX();
  rtl_and(&s1, &id_dest->val, &id_src->val);
  rtl_li(&s0, 0);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  rtl_update_ZFSF(&s1, id_dest->width);
  operand_write(id_dest, &s1);

  print_asm_template2(and);
}

make_EHelper(xor) {
  // XXX();
  rtl_xor(&s1, &id_dest->val, &id_src->val);
  rtl_li(&s0, 0);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  rtl_update_ZFSF(&s1, id_dest->width);
  operand_write(id_dest, &s1);

  print_asm_template2(xor);
}

make_EHelper(or) {
  // XXX();
  rtl_or(&s1, &id_dest->val, &id_src->val);
  rtl_li(&s0, 0);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  rtl_update_ZFSF(&s1, id_dest->width);
  operand_write(id_dest, &s1);

  print_asm_template2(or);
}

// 算术右移 (有符号)
make_EHelper(sar) {
  // XXX();
  // unnecessary to update CF and OF in NEMU
  rtl_li(&s0, id_dest->val & 0x01);
  rtl_set_CF(&s0);
  if (id_src->val == 1) {
    rtl_li(&s0, 0);
    rtl_set_OF(&s0);
  }
  rtl_sar(&s1, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&s1, id_dest->width);
  operand_write(id_dest, &s1);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  // XXX();
  // unnecessary to update CF and OF in NEMU
  rtl_li(&s0, id_dest->val & (0x01 << (id_dest->width * 8 - 1)));
  rtl_set_CF(&s0);
  rtl_shl(&s1, &id_dest->val, &id_src->val);
  if (id_src->val == 1) {
    rtl_get_CF(&s0);
    rtl_li(&s0, s0 ^ ((s1 & (0x01 << (id_dest->width * 8 - 1))) >> (id_dest->width * 8 - 1)));
    rtl_set_OF(&s0);
  }
  rtl_update_ZFSF(&s1, id_dest->width);
  operand_write(id_dest, &s1);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  // XXX();
  // unnecessary to update CF and OF in NEMU
  rtl_li(&s0, id_dest->val & 0x01);
  rtl_set_CF(&s0);
  rtl_shr(&s1, &id_dest->val, &id_src->val);
  if (id_src->val == 1) {
    rtl_li(&s0, s1 & (0x01 << (id_dest->width * 8 - 1)));
    rtl_set_OF(&s0);
  }
  rtl_update_ZFSF(&s1, id_dest->width);
  operand_write(id_dest, &s1);

  print_asm_template2(shr);
}

make_EHelper(rol) {
  // s0 存储着位宽
  switch (decinfo.opcode & 0xff) {
    case 0xd0:  // rol r/m8, 1
    case 0xd2:  // rol r/m8, cl
    case 0xc0: // rol r/m8, imm8
      s0 = 1;
      break;
    case 0xd1:  // rol r/m16/32, 1
    case 0xd3:  // rol r/m16/32, cl
    case 0xc1: // rol r/m16/32, imm8
      s0 = decinfo.isa.is_operand_size_16 ? 2 : 4;
      break;
    default:
      assert(0);
  }
  rtl_rol(&s1, &id_dest->val, id_src->val, s0);

  if (id_src->val == 1) {
    cpu.eflags.OF = (cpu.eflags.CF != (s1 >> (s0 * 8 - 1)));
  }

  operand_write(id_dest, &s1);

  print_asm_template1(rol);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  // XXX();
  rtl_not(&s1, &id_dest->val);
  operand_write(id_dest, &s1);

  print_asm_template1(not);
}

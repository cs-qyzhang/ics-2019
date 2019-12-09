#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  // XXX();
  switch (decinfo.opcode & 0xff) {
    case 0x0e:  // push cs
      rtl_li(&s0, cpu.CS);
      rtl_push(&s0);
      break;
    case 0x16:  // push ss
      rtl_li(&s0, cpu.SS);
      rtl_push(&s0);
      break;
    case 0x1e:  // push ds
      rtl_li(&s0, cpu.DS);
      rtl_push(&s0);
      break;
    case 0x06:  // push es
      rtl_li(&s0, cpu.ES);
      rtl_push(&s0);
      break;
    case 0xa0:  // push fs
      rtl_li(&s0, cpu.FS);
      rtl_push(&s0);
      break;
    case 0xa8:  // push gs
      rtl_li(&s0, cpu.GS);
      rtl_push(&s0);
      break;
    default:
      rtl_push(&id_dest->val);
      break;
  }

  print_asm_template1(push);
}

make_EHelper(pop) {
  // XXX();
  rtl_pop(&s1);
  switch (decinfo.opcode & 0xff) {
    case 0x1f:  // pop ds
      cpu.DS = s1;
      break;
    case 0x07:  // pop es
      cpu.ES = s1;
      break;
    case 0x17:  // pop ss
      cpu.SS = s1;
      break;
    case 0xa1:  // pop fs
      cpu.FS = s1;
      break;
    case 0xa9:  // pop gs
      cpu.GS = s1;
      break;
    default:
      operand_write(id_dest, &s1);
      break;
  }

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  // XXX();
  // FIXME: push 32bit or 16bit reg?
  int width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_lr(&s0, R_ESP, width);
  rtl_lr(&s1, R_EAX, width);
  rtl_push(&s1);
  rtl_lr(&s1, R_ECX, width);
  rtl_push(&s1);
  rtl_lr(&s1, R_EDX, width);
  rtl_push(&s1);
  rtl_lr(&s1, R_EBX, width);
  rtl_push(&s1);
  rtl_push(&s0);
  rtl_lr(&s1, R_EBP, width);
  rtl_push(&s1);
  rtl_lr(&s1, R_ESI, width);
  rtl_push(&s1);
  rtl_lr(&s1, R_EDI, width);
  rtl_push(&s1);

  print_asm("pusha");
}

make_EHelper(popa) {
  // XXX();
  // FIXME: pop 32bit or 16bit reg?
  int width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_pop(&s0);
  rtl_sr(R_EDI, &s0, width);
  rtl_pop(&s0);
  rtl_sr(R_ESI, &s0, width);
  rtl_pop(&s0);
  rtl_sr(R_EBP, &s0, width);
  rtl_pop(&s0);
  rtl_pop(&s0);
  rtl_sr(R_EBX, &s0, width);
  rtl_pop(&s0);
  rtl_sr(R_EDX, &s0, width);
  rtl_pop(&s0);
  rtl_sr(R_ECX, &s0, width);
  rtl_pop(&s0);
  rtl_sr(R_EAX, &s0, width);

  print_asm("popa");
}

make_EHelper(leave) {
  // XXX();
  rtl_lr(&s0, R_EBP, 4);
  rtl_sr(R_ESP, &s0, 4);
  rtl_pop(&s0);
  rtl_sr(R_EBP, &s0, 4);

  print_asm("leave");
}

// FIXME:这里原来写的是cwtl，应改为cwtd
// cwtd是at&t汇编的指令名，相当于intel的cwd: dx, ax <- signext(ax)
// cltd是at&t汇编的指令名，相当于intel的cdq: edx, eax <- signext(eax)
make_EHelper(cwtd) {
  // XXX();
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&s0, R_AX, 2);
    s1 = s0 < 0 ? -1 : 0;
    rtl_sr(R_DX, &s1, 2);
  }
  else {
    rtl_lr(&s0, R_EAX, 4);
    s1 = s0 < 0 ? -1 : 0;
    rtl_sr(R_EDX, &s1, 4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtd" : "cltd");
}

// cbtw是at&t汇编的指令名，相当于intel的cbw: ax <- signext(al)
// cwtl是at&t汇编的指令名，相当于intel的cwde: eax <- signext(ax)
make_EHelper(cwtl) {
  // XXX();
  if (decinfo.isa.is_operand_size_16) {
    rtl_lr(&s0, R_AL, 1);
    rtl_sext(&s1, &s0, 1);
    rtl_sr(R_AX, &s1, 2);
  }
  else {
    rtl_lr(&s0, R_AX, 2);
    rtl_sext(&s1, &s0, 2);
    rtl_sr(R_EAX, &s1, 4);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  id_src->width = ((decinfo.opcode & 0xff) == 0xbe) ? 1 : 2;  // 避免了在译码阶段写两个函数
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

// FIXME:不能够直接operand_write
make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  id_src->width = ((decinfo.opcode & 0xff) == 0xb6) ? 1 : 2;  // 避免了在译码阶段写两个函数
  if (id_src->width == 1) {
    s0 = id_src->val & 0xff;
  } else if (id_src->width == 2) {
    s0 = id_src->val & 0xffff;
  } else {
    s0 = id_src->val;
  }
  operand_write(id_dest, &s0);
  print_asm_template2(movzx);
}

make_EHelper(movsb) {
  int incdec = 1;
  switch (decinfo.opcode & 0xff) {
    case 0xa4:
      incdec = cpu.eflags.DF ? -1 : 1;
      rtl_lr(&s0, R_ESI, 4);
      rtl_lm(&s1, &s0, 1);
      s0 += incdec;
      rtl_sr(R_ESI, &s0, 4);
      rtl_lr(&s0, R_EDI, 4);
      rtl_sm(&s0, &s1, 1);
      s0 += incdec;
      rtl_sr(R_EDI, &s0, 4);
      print_asm("movsb");
      break;
    case 0xa5:
      if (decinfo.isa.is_operand_size_16) {
        incdec = cpu.eflags.DF ? -2 : 2;
        rtl_lr(&s0, R_ESI, 4);
        rtl_lm(&s1, &s0, 2);
        s0 += incdec;
        rtl_sr(R_ESI, &s0, 4);
        rtl_lr(&s0, R_EDI, 4);
        rtl_sm(&s0, &s1, 2);
        s0 += incdec;
        rtl_sr(R_EDI, &s0, 4);
        print_asm("movsw");
      } else {
        incdec = cpu.eflags.DF ? -4 : 4;
        rtl_lr(&s0, R_ESI, 4);
        rtl_lm(&s1, &s0, 4);
        s0 += incdec;
        rtl_sr(R_ESI, &s0, 4);
        rtl_lr(&s0, R_EDI, 4);
        rtl_sm(&s0, &s1, 4);
        s0 += incdec;
        rtl_sr(R_EDI, &s0, 4);
        print_asm("movsl");
      }
      break;
    default:
      panic("movs");
  }
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

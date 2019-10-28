#include "nemu.h"
#include <stdlib.h>
#include <time.h>
#include <strings.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

int isa_get_reg_index(char *reg) {
  for (int i = 0; i < 8; ++i) {
    if (!strcasecmp(reg, regsl[i])) {
      return i;
    } else if (!strcasecmp(reg, regsw[i])) {
      return i + 8;
    } else if (!strcasecmp(reg, regsb[i])) {
      return i + 16;
    }
  }
  Log("invalid register name: %s", reg);
  assert(0);
  return -1;
}

uint32_t isa_access_reg(int index) {
  assert(index >= 0 && index < 24);
  if (index < 8) {
    return reg_l(index);
  } else if (index < 16) {
    return reg_w(index - 8);
  } else {
    return reg_b(index - 16);
  }
}

void isa_reg_display(bool show_hex) {
  for (int i = 0; i < 4; ++i) {
    if (show_hex) {
      printf("%-5s%#10x\t%-5s%#10x\t%-5s%#10x\t%-s%#10x\n", regsl[i], reg_l(i),
          regsw[i], reg_w(i), regsb[i], reg_b(i), regsb[i + 4], reg_b(i + 4));
    } else {
      printf("%-5s%10d\t%-5s%10d\t%-5s%10d\t%-s%10d\n", regsl[i], reg_l(i),
          regsw[i], reg_w(i), regsb[i], reg_b(i), regsb[i + 4], reg_b(i + 4));
    }
  }
  for (int i = 4; i < 8; ++i) {
    if (show_hex) {
      printf("%-5s%#10x\t%-5s%#10x\n", regsl[i], reg_l(i), regsw[i], reg_w(i));
    } else {
      printf("%-5s%10d\t%-5s%10d\n", regsl[i], reg_l(i), regsw[i], reg_w(i));
    }
  }
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}

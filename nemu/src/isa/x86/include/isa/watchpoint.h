#ifndef __X86_WATCHPOINT_H__
#define __X86_WATCHPOINT_H__

#include "common.h"

// 在判断时由于不同位宽对应的寄存器可能有重叠，如ax和eax，为了方便判断寄存器
// 是否被修改这里只记录该寄存器对应的双字寄存器

// 寄存器的位掩码
extern const bool M_EAX;
extern const bool M_ECX;
extern const bool M_EDX;
extern const bool M_EBX;
extern const bool M_ESP;
extern const bool M_EBP;
extern const bool M_ESI;
extern const bool M_EBI;
extern const bool reg_mask[8];

// 表示当前寄存器是否是watchpoint的标志
typedef struct {
  bool eax  : 1;
  bool ecx  : 1;
  bool edx  : 1;
  bool ebx  : 1;
  bool esp  : 1;
  bool ebp  : 1;
  bool esi  : 1;
  bool edi  : 1;
} WP_REG_FLAG;

#endif // WATCHPOINT_H

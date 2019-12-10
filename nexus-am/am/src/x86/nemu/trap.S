#----|------------entry------------|---irq id---|-----handler-----|
.globl __am_vecsys;    __am_vecsys: pushl $0x80; jmp __am_asm_trap
.globl __am_vectrap;  __am_vectrap: pushl $0x81; jmp __am_asm_trap
.globl __am_irq0;        __am_irq0: pushl   $32; jmp __am_asm_trap
.globl __am_vecnull;  __am_vecnull: pushl   $-1; jmp __am_asm_trap

__am_asm_trap:
  pushal
  pushl $0            # AddressSpace
  pushl %esp          # _Context * 参数

  call __am_irq_handle

  addl $4, %esp       # _Context * 参数

  movl %eax, %esp     # 进行上下文切换，eax保存了 schedule() 返回的上下文指针

  addl $4, %esp       # AddressSpace
  popal
  addl $4, %esp

  iret

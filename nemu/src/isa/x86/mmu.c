#include "nemu.h"

// 注意 page_translate 要写在 nemu 中，不能够写在 AM 的 VME 中，这是因为
// page_translate 是用来读取虚拟机内存的，必须由虚拟机完成映射，而 AM 是
// 运行在虚拟机上的软件，若写在 AM 中，则 AM 在读取内存数据时又调用自己
// 来解析地址，这样会不断循环。
//
// 另外在这里的页表等都保存在虚拟机的内存中，所以需要用 paddr_read() 来获取
// 页表内容
inline paddr_t page_translate(vaddr_t va) {
  paddr_t ptab = paddr_read(cpu.cr3.val + sizeof(PDE) * PDX(va), sizeof(PDE));
  if (!(ptab & PTE_P)) {
    printf("ERROR:page_translate(): page table doesn't exists!\n");
    assert(0); 
  }

  // 注意这里从页目录中读出来的并不是真正的页表指针，需要将后面的标志位屏蔽掉！
  paddr_t pg = paddr_read(PTE_ADDR(ptab) + sizeof(PTE) * PTX(va), sizeof(PTE));
  if (!(pg & PTE_P)) {
    printf("ERROR:page_translate(): page doesn't exists!\n");
    assert(0); 
  }

  return (PTE_ADDR(pg) | OFF(va));
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if (cpu.cr0.paging) { // 开启分页
    if (PTE_ADDR(addr) != PTE_ADDR(addr + len - 1)) { // 数据跨页
      uint8_t byte[4];
      for (int i = 0; i < len; i++)
        byte[i] = isa_vaddr_read(addr + i, 1);
      if (len == 2)
        return *(uint16_t *)byte;
      else
        return *(uint32_t *)byte;
    } else {
      paddr_t paddr = page_translate(addr);
      return paddr_read(paddr, len);
    }
  } else {
    return paddr_read(addr, len);
  }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if (cpu.cr0.paging) { // 开启分页
    if (PTE_ADDR(addr) != PTE_ADDR(addr + len - 1)) { // 数据跨页
      uint8_t byte[4];
      if (len == 2)
        *(uint16_t *)byte = data;
      else
        *(uint32_t *)byte = data;
      for (int i = 0; i < len; i++)
        isa_vaddr_write(addr + i, byte[i], 1);
    } else {  // 数据没有跨页
      paddr_t paddr = page_translate(addr);
      paddr_write(paddr, data, len);
    }
  } else {  // 不分页
    paddr_write(addr, data, len);
  }
}

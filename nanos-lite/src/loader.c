#include "proc.h"
#include "fs.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define min(x, y)         ((x) < (y) ? (x) : (y))
#define PTE_ADDR(pte)     ((uint32_t)(pte) & ~0xfff)
#define OFF(va)           ((uint32_t)(va) & 0xfff)

static uintptr_t loader(PCB *pcb, const char *filename) {
  // XXX();
  int fd = fs_open(filename, 0, 0);
  if (fd == -1) {
    panic("loader: can't open file %s!", filename);
  }

  Elf_Ehdr elf_header;
  fs_read(fd, (void *)&elf_header, sizeof(Elf_Ehdr));
  if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG))
    panic("file %s ELF format error!", filename);

  for (size_t i = 0; i < elf_header.e_phnum; ++i) {
    Elf_Phdr phdr;
    fs_lseek(fd, elf_header.e_phoff + elf_header.e_phentsize * i, SEEK_SET);
    fs_read(fd, (void *)&phdr, elf_header.e_phentsize);
    if (phdr.p_type == PT_LOAD) {
      fs_lseek(fd, phdr.p_offset, SEEK_SET);

#ifdef HAS_VME
      // 为用户进程申请虚存空间
      void *vaddr = (void *)phdr.p_vaddr;
      void *paddr;
      int32_t left_file_size = phdr.p_filesz;

      // 这里注意在映射后由于没有修改CR3寄存器，所以现在的映射还没有启用，在
      // fs_read和memset时要对物理地址进行修改而不是虚拟地址

      // 处理第一页 (第一页可能不是页对齐)
      paddr = new_page(1);
      _map(&pcb->as, vaddr, paddr, 0);
      uint32_t page_write_size = min(left_file_size, PTE_ADDR((uint32_t)vaddr + PGSIZE) - (uint32_t)vaddr);
      fs_read(fd, (void *)(PTE_ADDR(paddr) | OFF(vaddr)), page_write_size);
      left_file_size -= page_write_size;
      vaddr += page_write_size;
      for (; left_file_size > 0; left_file_size -= page_write_size, vaddr += page_write_size) {
        assert(((uint32_t)vaddr & 0xfff) == 0);
        paddr = new_page(1);
        _map(&pcb->as, vaddr, paddr, 0);
        page_write_size = min(left_file_size, PGSIZE);
        fs_read(fd, paddr, page_write_size);
      }

      // 将进程剩下的地址空间赋值为0
      left_file_size = phdr.p_memsz - phdr.p_filesz;
      if (((uint32_t)vaddr & 0xfff) != 0) {
        page_write_size = min(left_file_size, PTE_ADDR((uint32_t)vaddr + PGSIZE) - (uint32_t)vaddr);
        memset((void *)(PTE_ADDR(paddr) | OFF(vaddr)), 0, page_write_size);
        left_file_size -= page_write_size;
        vaddr += page_write_size;
      }
      for (page_write_size = PGSIZE; left_file_size > 0; left_file_size -= page_write_size, vaddr += page_write_size) {
        assert(((uint32_t)vaddr & 0xfff) == 0);
        paddr = new_page(1);
        _map(&pcb->as, vaddr, paddr, 0);
        memset(paddr, 0, page_write_size);
      }
#else
      fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
      memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
#endif
    }
  }

  fs_close(fd);
  printf("%s: %x\n", filename, elf_header.e_entry);

  return elf_header.e_entry;
}

static uintptr_t kloader(PCB *pcb, const char *filename) {
  // XXX();
  int fd = fs_open(filename, 0, 0);
  if (fd == -1) {
    panic("loader: can't open file %s!", filename);
  }

  Elf_Ehdr elf_header;
  fs_read(fd, (void *)&elf_header, sizeof(Elf_Ehdr));
  if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG))
    panic("file %s ELF format error!", filename);

  for (size_t i = 0; i < elf_header.e_phnum; ++i) {
    Elf_Phdr phdr;
    fs_lseek(fd, elf_header.e_phoff + elf_header.e_phentsize * i, SEEK_SET);
    fs_read(fd, (void *)&phdr, elf_header.e_phentsize);
    if (phdr.p_type == PT_LOAD) {
      fs_lseek(fd, phdr.p_offset, SEEK_SET);
      fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
      memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
    }
  }

  fs_close(fd);
  printf("load file: %s, entry: %x\n", filename, elf_header.e_entry);

  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = kloader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  memset(pcb, 0, sizeof(PCB));  // 用于在调用execve时将原有进程保存的max_brk等信息清空
#ifdef HAS_VME
  _protect(&pcb->as); // 建立用户进程虚存空间中的内核映射
#endif

  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}

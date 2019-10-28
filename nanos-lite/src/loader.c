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
      fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
      memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
    }
  }

  fs_close(fd);

  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
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
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}

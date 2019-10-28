#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"

static inline int32_t sys_open(const char *path, int flags, int mode) {
  return fs_open(path, flags, mode);
}

static inline int32_t sys_read(int fd, void *buf, size_t len) {
  return fs_read(fd, buf, len);
}

static inline int32_t sys_write(int fd, void *buf, size_t len) {
  return fs_write(fd, buf, len);
}

static inline int32_t sys_lseek(int fd, uint32_t offset, int whence) {
  return fs_lseek(fd, offset, whence);
}

static inline int32_t sys_close(int fd) {
  return fs_close(fd);
}

static inline int32_t sys_execve(const char *pathname, char *const argv[], char *const envp[]) {
  naive_uload(NULL, pathname);
  return 0;
}

static inline int32_t sys_exit(int32_t status) {
  return sys_execve("/bin/init", NULL, NULL);
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit:
      sys_exit(a[1]);
      break;
    case SYS_yield:
      _yield();
      c->GPRx = 0;
      break;
    case SYS_open:
      c->GPRx = sys_open((void *)a[1], a[2], a[3]);
      break;
    case SYS_read:
      c->GPRx = sys_read(a[1], (void *)a[2], a[3]);
      break;
    case SYS_write:
      c->GPRx = sys_write(a[1], (void *)a[2], a[3]);
      break;
    case SYS_kill:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_getpid:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_close:
      c->GPRx = sys_close(a[1]);
      break;
    case SYS_lseek:
      c->GPRx = sys_lseek(a[1], a[2], a[3]);
      break;
    case SYS_brk: // 总是成功
      c->GPRx = 0;
      break;
    case SYS_fstat:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_time:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_signal:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_execve:
      c->GPRx = sys_execve((void *)a[1], (char *const *)(void *)a[2], (char *const *)(void *)a[3]);
      break;
    case SYS_fork:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_link:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_unlink:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_wait:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_times:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    case SYS_gettimeofday:
      panic("Unhandled syscall ID = %d", a[0]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}

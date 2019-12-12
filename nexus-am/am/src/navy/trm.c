#include <am.h>
#include <stdio.h>

/*_Area _heap = {};*/

/*void _trm_init() {*/
/*}*/

/*void _putc(char ch) {*/
/*}*/

/*void _halt(int code) {*/
  /*while (1);*/
/*}*/
extern char _heap_start;
extern char _heap_end;
int main(const char *args);

_Area _heap = {
  .start = &_heap_start,
  .end = &_heap_end,
};

void _putc(char ch) {
  putchar(ch);
}

void _halt(int code) {
  extern void _exit(int status);
  _exit(code);

  // should not reach here
  while (1);
}

void _trm_init() {
  extern const char _start;
  const char *mainargs = &_start - 0x100000;
  int ret = main(mainargs);
  _halt(ret);
}

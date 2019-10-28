#include "common.h"
#include <stdarg.h>

FILE *log_fp = NULL;
FILE *call_fp = NULL;

static char log_name[250];
static char call_log_file[256];

void init_log(const char *log_file) {
  strcpy(log_name, log_file);

  Log("log_file location: %s", log_file);
  if (log_file == NULL) return;
  log_fp = fopen(log_file, "w");
  Assert(log_fp, "Can not open '%s'", log_file);

  sprintf(call_log_file, "%s.call", log_file);
  Log("call_log_file location: %s", call_log_file);
  call_fp = fopen(call_log_file, "w");
  Assert(call_fp, "Can not open '%s'", call_log_file);
}

void reopen_log() {
  Log("log_file location: %s", log_name);
  log_fp = fopen(log_name, "w");
  Assert(log_fp, "Can not open '%s'", log_name);
}

void reopen_call_log() {
  Log("call_log_file location: %s", call_log_file);
  call_fp = fopen(call_log_file, "w");
  Assert(call_fp, "Can not open '%s'", call_log_file);
}

char log_bytebuf[80] = {};
char log_asmbuf[80] = {};
static char tempbuf[256] = {};

void strcatf(char *buf, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(tempbuf, sizeof(tempbuf), fmt, ap);
  va_end(ap);
  strcat(buf, tempbuf);
}

void asm_print(vaddr_t ori_pc, int instr_len, bool print_flag) {
  snprintf(tempbuf, sizeof(tempbuf), "%8x:   %s%*.s%s", ori_pc, log_bytebuf,
      50 - (12 + 3 * instr_len), "", log_asmbuf);
  log_write("%s\n", tempbuf);
  if (print_flag) {
    puts(tempbuf);
  }

  log_bytebuf[0] = '\0';
  log_asmbuf[0] = '\0';
}

#include "common.h"
#include "proc.h"
#include <amdev.h>

extern void _yield();

size_t serial_write(const void *buf, size_t offset, size_t len) {
  _yield(); // 模拟IO慢，进行调度

  for (size_t i = 0; i < len; ++i)
    _putc(((char *)buf)[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

#define KEYDOWN_MASK 0x8000

size_t events_read(void *buf, size_t offset, size_t len) {
  _yield(); // 模拟IO慢，进行调度

  int keycode = read_key();
  if ((keycode & ~KEYDOWN_MASK) == _KEY_NONE) {
    sprintf(buf, "t %d\n", uptime());
  } else if (keycode & KEYDOWN_MASK) {
    sprintf(buf, "kd %s\n", keyname[keycode & ~KEYDOWN_MASK]);
    if (keyname[keycode & ~KEYDOWN_MASK][0] == 'F') {
      Log("F key down!");
      switch (keyname[keycode & ~KEYDOWN_MASK][1]) {
        case '1':
          fg_pcb = &pcb[1];
          break;
        case '2':
          fg_pcb = &pcb[2];
          break;
        case '3':
          fg_pcb = &pcb[3];
          break;
        default:
          break;
      }
    }
  } else {
    sprintf(buf, "ku %s\n", keyname[keycode & ~KEYDOWN_MASK]);
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};
static int screen_h, screen_w;

size_t get_dispinfo_size() {
  return strlen(dispinfo);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  strncpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  _yield(); // 模拟IO慢，进行调度

  int x, y;
  assert(offset + len <= (size_t)screen_h * screen_w * 4);
  x = (offset / 4) % screen_w;
  y = (offset / 4) / screen_w;
  assert(x + len < (size_t)screen_w * 4);
  draw_rect((void *)buf, x, y, len / 4, 1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  //fb_write(buf, offset, len);
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // XXX: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  screen_h = screen_height();
  screen_w = screen_width();
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_w, screen_h);
}

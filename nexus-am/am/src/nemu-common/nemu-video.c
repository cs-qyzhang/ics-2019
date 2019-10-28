#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>

static int width, height;

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      uint32_t data = inl(SCREEN_ADDR);
      info->height = data & 0xffffu;
      info->width = data >> 16;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;

      uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
      for (int row = 0; row < ctl->h; ++row)
        memcpy(&fb[(row + ctl->y) * width + ctl->x], &ctl->pixels[row * ctl->w], ctl->w * 4);

      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  width = screen_width();
  height = screen_height();
  int size = width * height;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}

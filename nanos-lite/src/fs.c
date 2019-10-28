#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t fbsync_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
  {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  {"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write}, // 保证/dev/fb文件是数组的最后一个
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // XXX: initialize the size of /dev/fb
  file_table[NR_FILES - 1].size = screen_width() * screen_height() * 4;
  file_table[NR_FILES - 2].size = 1;
  extern size_t get_dispinfo_size();
  file_table[NR_FILES - 3].size = get_dispinfo_size();
}

int32_t fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < NR_FILES; ++i) {
    if (!strcmp(pathname, file_table[i].name)) {
      return i;
    }
  }
  return -1;
}

static inline void fd_check(int fd) {
  assert(0 <= fd && fd < NR_FILES);
}

int32_t fs_read(int fd, void *buf, size_t len) {
  fd_check(fd);
  size_t sz;
  if (file_table[fd].read == NULL) {
    sz = file_table[fd].open_offset + len <= file_table[fd].size ? len : file_table[fd].size - file_table[fd].open_offset;
    sz = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, sz);
    file_table[fd].open_offset += sz;
    return sz;
  } else {
    sz = len;
    if (file_table[fd].size && file_table[fd].open_offset + len > file_table[fd].size) {
      sz = file_table[fd].size - file_table[fd].open_offset;
    }
    sz = file_table[fd].read(buf, file_table[fd].open_offset, sz);
    file_table[fd].open_offset += sz;
    return sz;
  }
}

int32_t fs_write(int fd, const void *buf, size_t len) {
  fd_check(fd);
  size_t sz;
  if (file_table[fd].write == NULL) {
    sz = file_table[fd].open_offset + len <= file_table[fd].size ? len : file_table[fd].size - file_table[fd].open_offset;
    sz = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, sz);
    file_table[fd].open_offset += sz;
    return sz;
  } else {
    sz = len;
    if (file_table[fd].size && file_table[fd].open_offset + len > file_table[fd].size) {
      sz = file_table[fd].size - file_table[fd].open_offset;
    }
    sz = file_table[fd].write(buf, file_table[fd].open_offset, sz);
    file_table[fd].open_offset += sz;
    return sz;
  }
}

uint32_t fs_lseek(int fd, uint32_t offset, int whence) {
  fd_check(fd);
  switch (whence) {
    case SEEK_SET:
      file_table[fd].open_offset = offset;
      break;
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      break;
    case SEEK_END:
      file_table[fd].open_offset = file_table[fd].size + offset;
      break;
    default:
      panic("lseek whence error!");
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  file_table[fd].open_offset = 0;
  return 0;
}

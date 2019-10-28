#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t sz;
  for (sz = 0; s[sz] != '\0'; ++sz) ;
  return sz;
}

char *strcpy(char* dst,const char* src) {
  size_t i;
  for (i = 0; src[i] != '\0'; ++i)
    dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; ++i)
    dst[i] = src[i];
  for (; i < n; ++i)
    dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  size_t dst_len = strlen(dst);
  size_t i;
  for (i = 0; src[i] != '\0'; ++i)
    dst[dst_len + i] = src[i];
  dst[dst_len + i] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i;
  for (i = 0; s1[i] == s2[i]; ++i) {
    if (s1[i] == '\0')
      return 0;
  }
  return ((signed char *)s1)[i] - ((signed char *)s2)[i];
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t i;
  for (i = 0; i < n && s1[i] == s2[i]; ++i) {
    if (s1[i] == '\0')
      return 0;
  }
  return (i == n) ? 0 : (((signed char *)s1)[i] - ((signed char *)s2)[i]);
}

void* memset(void* v,int c,size_t n) {
  size_t i;
  for (i = 0; i < n; ++i)
    ((int8_t *)v)[i] = c;
  return v;
}

void* memcpy(void* dst, const void* src, size_t n) {
  assert(dst - src >= n || dst - src >= n);
  size_t i;
  for (i = 0; i < n; ++i)
    ((int8_t *)dst)[i] = ((int8_t *)src)[i];
  return dst;
}

int memcmp(const void* s1, const void* s2, size_t n){
  size_t i;
  for (i = 0; i < n; ++i)
    if (((unsigned char *)s1)[i] != ((unsigned char *)s2)[i])
      return ((signed char *)s1)[i] - ((signed char *)s2)[i];
  return 0;
}

#endif

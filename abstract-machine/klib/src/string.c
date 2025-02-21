#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  size_t i = 0;
  for(i = 0; src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i = 0;
  for(i = 0; i < n && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  for(; i < n; i++) {
    dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t len = strlen(dst);
  size_t i = 0;
  for(i = 0; src[i] != '\0'; i++) {
    dst[len + i] = src[i];
  }
  dst[len + i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  assert(s1&&s2);
  for (int i = 0; s1[i] != '\0' || s2[i] != '\0'; i++) {
		if (s1[i] != s2[i]) {
  			return *s1-*s2;
  		}
	}
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  const char* p1 = s1;
  const char* p2 = s2;
  assert(p1 && p2);
  if (!n) return 0; // 如果比较长度为0，直接返回0
  while (--n && *p1 && (*p1 == *p2)) {
    p1++;
    p2++;
}
  return *p1 - *p2; // 返回第一个不匹配字符的差值
}

void *memset(void *s, int c, size_t n) {
  assert(s);
  unsigned char *p = s;
  while(n--) {
    *p++ = (unsigned char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  unsigned char*p = dst;
  while(n--){
    *p++ = *(unsigned char*)src++;
  }
  return dst;
}

//TODO:处理内存重叠和进行优化
void *memcpy(void *out, const void *in, size_t n) {
  assert(out && in);
  char*pout = out;
  const char*pin = in;
  while(n--){
    *pout++ = *pin++;
  }
  return out;
  
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *ps1 = (const unsigned char *)s1;
  const unsigned char *ps2 = (const unsigned char *)s2;
  while (n--) {
    if (*ps1 != *ps2) {
    return *ps1 - *ps2;
    }
    ps1++;
    ps2++;
  }
  return 0;
}

#endif

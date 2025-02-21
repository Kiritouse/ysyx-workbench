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
  for (int i = 0; s1[i] != '\0' || s2[i] != '\0'; i++) {
		if (s1[i] != s2[i]) {
  			return (s1[i] < s2[i]) ? -1 : 1;
  		}
	}
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  for (int i = 0; i < n; i++) {
    if (s1[i] != s2[i]) {
  			return (s1[i] < s2[i]) ? -1 : 1;
  		}
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  for(int i = 0;i<n;i++){
    ((char *)s)[i] = c; //转为按字节操作，然后每个字节都赋值为c
  }
  return NULL;
}

void *memmove(void *dst, const void *src, size_t n) {
  for(int i = 0;i<n;i++){
    ((char *)dst)[i] = ((char *)src)[i];
  }
  return NULL;
}

void *memcpy(void *out, const void *in, size_t n) {
  for(int i = 0;i<n;i++){
    ((char *)out)[i] = ((char *)in)[i];
  }
  return NULL;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  for(int i = 0;i<n;i++){
    if(((char *)s1)[i] != ((char *)s2)[i]){
      return ((char *)s1)[i] < ((char *)s2)[i] ? -1 : 1;
    }
  }
  return 0;
}

#endif

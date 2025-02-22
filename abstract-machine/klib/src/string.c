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
  for (size_t i = 0; s1[i] != '\0' || s2[i] != '\0'; i++) {
		if (s1[i] != s2[i]) {
  			return s1[i]-s2[i];
  		}
	}
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  assert(s1 && s2);
  for (size_t i = 0; i < n; i++) {
    if (s1[i] == '\0' || s2[i] == '\0' || s1[i] != s2[i]) {
        if (s1[i] == s2[i]) {
            return 0;
        }
        return s1[i] - s2[i];
    }
}
return 0;

}

void *memset(void *s, int c, size_t n) {
  assert(s);
   char *p = (char*)s;
  while(n--) {
    *p++ = c;
  }
  return s;
}

/*
Copies the values of num bytes from the location pointed by source to the memory block pointed by destination. Copying takes place as if an intermediate buffer were used, allowing the destination and source to overlap.
从source（源内存块位置）直接指向的地方开始复制num个字节的数据到destination指向的内存块位置。然后复制就像使用了中间缓冲区一样，允许destination和source重叠。

The function does not check for any terminating null character in source - it always copies exactly num bytes.
这个函数不会检查'\0'，不会遇到'\0'就停下来，它就只认识要复制的num个字节数据。

To avoid overflows, the size of the arrays pointed by both the destination and source parameters, shall be at least num bytes.
为了避免溢出，这两个数组的大小至少为num个字节。 
避免memcpy导致的重叠部分被覆盖的情况
如
*/
void *memmove(void *dst, const void *src, size_t n) {
	int i;
	char *dest = (char *)dst;
	char *source = (char *)src;
	if(dst <= src){
		for(i = 0;i < n;i++){
			*dest++ = *source++;
		}
	}
	else{ //从后往前
		dest += n-1;
		source += n-1;
		for(i = 0;i < n;i++){
			*dest-- = *source--;
		}
	}
	return (void *)dst;
}
/*
The function does not check for any terminating null character in source - it always copies exactly num bytes.

这个函数不会检查'\0'，不会遇到'\0'就停下来，它就只认识要复制的num个字节数据。

To avoid overflows, the size of the arrays pointed to by both the destination and source parameters, shall be at least num bytes, and should not overlap (for overlapping memory blocks, memmove is a safer approach).

为了避免溢出，这两个数组的大小至少为num个字节，而且这两个数组内存位置不应该重叠。
*/
void *memcpy(void *out, const void *in, size_t n) {
  assert(out && in);
  char*pout =(char*) out;
  char*pin = (char*)in;
  while(n--){
    *pout++ = *pin++;
  }
  return out;
  
}


//
int memcmp(const void *s1, const void *s2, size_t n) {
	unsigned char *p1 = (unsigned char *)s1;
	unsigned char *p2 = (unsigned char *)s2;
	int i;
	for(i = 0; i < n ; i++)
		if(p1[i] != p2[i])
			return (p1[i] < p2[i]) ? -1 : 1;
	return 0;
}

#endif

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define MAXDEC 64

static char *__out;
void sputch(char ch){*__out++ = ch;}

int vprintf( void(*gputch)(char) , const char *fmt, va_list ap){ //返回输出的字符数
	int i;
	bool in_format = false;
	int long_flags = 0;//处理%ld这种大于一个字符的占位符
	int cnt = 0;
	for( ;*fmt != '\0';fmt++){
		if(*fmt != '%' && in_format == false){//如果是普通字符，调用打印函数将字符放入输出字符串即可
			gputch(*fmt);cnt++;
		}
		else{//如果是占位符，那么就根据%后面的类型进行判断，则得先跳过%字符，并且设置in_format为true，是为了表示进入了占位符替换模式的状态
			if(in_format == false && (*fmt == '%')){
				fmt++;//跳过%
				in_format = true;
			}
			switch(*fmt){
				case 'l':  //para
					long_flags += 1;
					break;
				case 's':  //%s
					char *s;
					assert(long_flags == 0);
					s = va_arg(ap , char *);
					for(i = 0; s[i] != '\0'; i++){
						gputch(s[i]);cnt++;
					}
					in_format = false;
					break;
				case 'c':  //%c
					int c;
					assert(long_flags == 0);//只有一位的情况
					c = va_arg(ap , int);//获取字符的ascii码
					gputch((char)c);cnt++;
					in_format = false;
					break;
				case 'd':{//%d或者%ld
					assert(long_flags <= 2);
					int64_t d = 0;
					if(long_flags == 2)    //%ld
						d = va_arg(ap , int64_t);
					else                   //%d
						d = va_arg(ap , int32_t);
          
            //处理负数和0的情况
					if(d < 0){
						d = -d;
						gputch('-');cnt++;
					}
					if(d == 0){
						gputch('0');cnt++;
					};
          //将数字转换为字符串
					char invert[MAXDEC];
					i = 0;
					for( ; d != 0 ; i++ , d/=10){
						invert[i] = d%10 + '0';
					}
					for(i-=1 ;i >= 0 ; i--){
						gputch(invert[i]);cnt++;
					}
					long_flags = 0;
					in_format = false;
					break;
					}
				case 'u':{  //%u
					uint64_t u = 0;
					assert(long_flags <= 2);
					if(long_flags == 2)   //%lu,无符号十进制六十四位整数
						u = va_arg(ap , uint64_t);
					else //%u 无符号十进制三十二位整数
						u = va_arg(ap , uint32_t);

					if(u == 0){
						gputch('0');cnt++;
					};
					char invert[MAXDEC];
					i = 0;
					for( ; u != 0 ; i++ , u/=10){
						invert[i] = u%10 + '0';
					}
					for(i-=1 ;i >= 0 ; i--){
						gputch(invert[i]);cnt++;
					}
					long_flags = 0;
					in_format = false;
					break;
					}
				case '%':
					gputch('%');
					in_format = false;
					break;
			}
		}
	}
	return cnt;
}

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
	va_start(ap, fmt); //将ap指向fmt后面的参数
	__out = out;//将结果返回到out中
	int cnt = vprintf(sputch , fmt , ap);
	sputch('\0');
	va_end(ap);
	return cnt++;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif

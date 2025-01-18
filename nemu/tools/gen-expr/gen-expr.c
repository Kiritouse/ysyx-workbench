/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#define MAX_DEPTH 250
// this should be enough
static char buf[65536+13] = {};
static char code_buf[65536 +13+ 128] = {}; // a little larger than `buf` 
static int depth = 0;
static int iserr = 0;
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static unsigned long int len_buf = 0;
static int choose(int n){
  return rand()%n;
}
static void gen_num(){
  int num = choose(9)%9+1; //生成1-9
  if(len_buf+7<sizeof(buf)){
    buf[len_buf++] = num+'0';
  }
  else return;
}
static void gen_rand_op(){
  char ops[] = {'+','-','*','/'};
  int index = choose(4);//随机选择生成一个字符
  if(len_buf+3<sizeof(buf)){
    buf[len_buf++] = ops[index];
  }
  else return;
}
static void gen_rand_expr() { //似乎存在栈溢出
  if(iserr)return;
  if(depth>=MAX_DEPTH) return;
  depth++;
  switch(choose(3)){
    case 0: //处理生成数字的情况
      if(len_buf==0){ //保证第一个字符不是运算符或者其他符号
        gen_num(); 
      }
      else{
        if(buf[len_buf-1]!=')'&&
        len_buf+7<sizeof(buf)) //因为右括号后不能直接跟数字，因此，如果上一个不是右括号，那么就可以直接生成数字
        gen_num();
        else gen_rand_expr();
      }
      break;
    case 1://生成(...)的情况
      if(len_buf==0){//如果里面还没有字符，那么可以生成( 
        buf[len_buf++] = '(';
        gen_rand_expr();
        if(len_buf+3>=sizeof(buf)){ //留出一定空间，防止莫名其妙的栈溢出
          iserr = 1;
          depth--;
          return;
        }
        buf[len_buf++] = ')';
      }
      else{
        //生成类似  ... op (...) 
        if (buf[0] != '\0' &&  strchr("+-*/", buf[len_buf-1])  //如果上一个位置出现的是符号op
          &&len_buf+3<sizeof(buf)){   
          buf[len_buf++] = '(';
          gen_rand_expr(); 
          if(len_buf+3>=sizeof(buf)){
            iserr= 1;
            depth--;
            return;
          }
          buf[len_buf++] = ')';
        }
        else gen_rand_expr();
      }
      break;
    default:
      gen_rand_expr();
      gen_rand_op();
      gen_rand_expr();
      break;
  }
  depth--;
}
int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    iserr = 0;
    depth = 0;
    len_buf = 0;
    memset(buf,0,sizeof(buf)); //这里如果不清空的话，每次循环就会出现问题,容易segmentation fault
    memset(code_buf,0,sizeof(code_buf));
    long file_size = 0;
    gen_rand_expr();
    if(iserr){
      printf("有缓冲区溢出\n");
      if(i>0);
      i--;
      continue;
    }
    buf[len_buf] = '\0';
    snprintf(code_buf,strlen(code_format)+len_buf,code_format, buf);
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fseek(fp,0,SEEK_END);
    fclose(fp);
    FILE*fp_err = fopen("/tmp/.err_msg_code","w");
    assert(fp_err!=NULL);
    int ret = system("gcc /tmp/.code.c -o /tmp/.expr 2> /tmp/.err_msg_code"); //将错误的信息重定向到错误文件中去
    if (ret != 0) {
      if(i!=0)
      i--; 
      continue;
    }
    fseek(fp_err,0,SEEK_END);//将文件指针移动到文件末尾
    //获取错误信息文件的大小
    file_size = ftell(fp_err);
    fclose(fp_err);
    if(file_size!=0){
      if(i>0)
      i--;
      continue;
    }
    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);
    int result;
    ret = fscanf(fp, "%d", &result);//获取程序运行的正确的结果
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}

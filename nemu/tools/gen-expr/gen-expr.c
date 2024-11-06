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
typedef uint32_t word_t;
// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static word_t choose(word_t n){
  return rand()%n;
}
static void gen_num(){
  word_t num = choose(9)%9+1; //生成1-9
  char temp_num_str_buf[2]; //多储存一个'\0'
  sprintf(temp_num_str_buf,"%d",num);
  temp_num_str_buf[1] = '\0';
  if(strlen(buf)+strlen(temp_num_str_buf)<sizeof(buf)){
    strcat(buf,temp_num_str_buf);//拼接
  }
  else return;
}
static void gen_rand_op(){
  char ops[] = {'+','-','*','/'};
  word_t index = choose(4);//随机选择生成一个字符
  char op_buf[2] = {ops[index],'\0'};
  if(strlen(buf)+strlen(op_buf)<sizeof(buf)){
    strcat(buf,op_buf);
  }
  else return;
}
static void gen_rand_expr() {
  if(strlen(buf)>65530){
    return;
  }
  switch(choose(3)){
    case 0: //处理生成数字的情况
        if(buf[strlen(buf)-1]!=')')
        gen_num();
        else gen_rand_expr();
      break;
    case 1:
      if (buf[0] != '\0' &&  strchr("+-*/", buf[strlen(buf) - 1])){
        if(strlen(buf)+7<sizeof(buf)){
          strcat(buf,"(");
          gen_rand_expr();
         strcat(buf,")");
        }
      }
      else gen_rand_expr();
      break;
    default:
      gen_rand_expr();
      gen_rand_op();
      gen_rand_expr();
      break;
  }
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
    memset(buf,0,sizeof(buf)); //这里如果不清空的话，每次循环就会出现问题,容易segmentation fault
    memset(code_buf,0,sizeof(code_buf));
    long file_size = 0;
    gen_rand_expr();
    sprintf(code_buf, code_format, buf);
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    FILE*fp_err = fopen("/tmp/.err_msg_code","w");
    assert(fp_err!=NULL);


    int ret = system("gcc /tmp/.code.c -o /tmp/.expr 2> /tmp/.err_msg_code"); //将错误的信息重定向到错误文件中去
    if (ret != 0) {
      i--; 
      continue;
    }
    fseek(fp_err,0,SEEK_END);

    //获取错误信息文件的大小
    file_size = ftell(fp_err);
    fclose(fp_err);
    if(file_size!=0){
      i--;
      continue;
    }
    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}

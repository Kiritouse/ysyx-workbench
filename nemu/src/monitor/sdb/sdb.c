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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"


static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
bool new_wp(char*args);
bool free_wp(int _NO);
void display_wp();
word_t vaddr_read(vaddr_t addr, int len);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}



/*my implement */
static int cmd_si(char*args){ //step
    int step = 0;
    if(args==NULL) step = 1;
    else
      sscanf(args,"%d",&step);

    if(step<0){
      printf("invalid args\n");
    }
    else
      cpu_exec(step);
    return 0;
} 

static int cmd_info(char*args){
  if(args==NULL){
    printf("No args,please input the agrs r or w\n");
  }
  else if(strcmp(args,"r")==0){ //打印寄存器状态
    isa_reg_display();
  }
  else if(strcmp(args,"w")==0){ //打印监视点信息
    printf("打印监视点信息暂时还没有实现\n");
    display_wp();
  }
  return 0;
}

static int cmd_p(char*args){
  if(args==NULL){
    printf("No args\n");
  }
  else{
    bool success = true;
    word_t ans = 0;
     ans =  expr(args,&success);
     if(success!=true){  //TODO：这里似乎有bug，不能识别错误？
      printf("expr false\n");
     }
     else{
      printf("expr = %d\n",ans);
     }
  }
  return 0;
}

static int cmd_x(char*args){
  char* N_byte = strtok(NULL," ");//N个4字节
  char*address_str = strtok(NULL," ");//十六进制地址，0x开头

  int N = sscanf(N_byte,"%d",&N);
  vaddr_t address;
  sscanf(address_str,"%x",&address);
  for(int i = 0;i<N;i++){
    printf("0x%08x: 0x%08x\n",address,vaddr_read(address,4));
    address+=4;
  }
  return 0;
}
static int cmd_w(char*args){
  bool ret  = true;
  ret = new_wp(args);
  if(!ret){
    printf("cmd_w error\n");
  }
  return 0;
}
static int cmd_d(char*args){
  bool ret = true;
  ret = free_wp(atoi(args));
  if(!ret){
    printf("cmd_d error\n");
  }
  return 0;
}


static int cmd_test_expr(char*args){ //test file_path
  int right_cnt = 0;
  FILE *input_file = fopen(args,"r");
    if(input_file==NULL){
      printf("file open fail,please check the path of file");
    }
    char line_data[1024*4] = {};
      unsigned int cor_val = 0;
    char buf[1024*4] = {};

    for(int i = 0;i<100;i++){
      memset(line_data,0,sizeof(line_data));
      memset(buf,0,sizeof(buf));
      if(fgets(line_data,sizeof(line_data),input_file)==NULL){
          perror("read data error");
          break;
      }
      char* token = strtok(line_data," ");
      if(token == NULL){
        printf("read correct val error");
        continue;
      }
      cor_val = atoi(token);
      while((token = strtok(NULL,"\n"))){
        strcat(buf,token);
      }
      printf("value : %u, express: %s\n",cor_val,buf);
      bool success = true;
      unsigned res = (unsigned int)expr(buf,&success);
      if(res==cor_val)right_cnt++;
    }
    printf("test 100 expressions,the accuracy is %d/100\n",right_cnt);
    fclose(input_file);
    return 0;

}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *); //函数指针，参数是一个char*，返回值是int
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","step n steps",cmd_si},
   {"info","info r,Display all registers,info w display all watchpoints ",cmd_info},
    {"x","x N address,Displays an offset of N bytes based on addressisplay the address",cmd_x},
    {"p","p expr,it will calculate expr and print the answer of expr",cmd_p},
    {"test","test file_path,test accu of eval function",cmd_test_expr},
    {"w","w expr,set up a watchpoint",cmd_w},
    {"d","d NO,delete a watchpoint",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

/*指令读取的地方 */
void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " "); //返回第一个被分割的指令，同时内部储存一个静态指针,指向上一次分割后的下一个字符串
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1; //指向指令的具体内容
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}

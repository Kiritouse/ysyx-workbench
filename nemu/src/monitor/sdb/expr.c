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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_NUM,
  TK_LEFT_BRACKET,TK_RIGHT_BRACKET,
  /* TODO: Add more token types */
  TK_MINUS_SIGN,

};

static struct rule {
  const char *regex; //正则表达式
  int token_type;//token类型
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // sub
  {"\\*", '*'},         // mul
  {"\\/", '/'},         // div
  {"\\(",TK_LEFT_BRACKET},//左括号
  {"\\)",TK_RIGHT_BRACKET},//右括号
  {"\\b[0-9]+\\b", TK_NUM},//num
  {"==", TK_EQ},        // equal
};


#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      //按顺序尝试用不同的规则来匹配当前位置的字符串. 当一条规则匹配成功, 并且匹配出的子串正好是position所在位置的时候, 我们就成功地识别出一个token
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) { //position为当前处理到的位置
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        assert(substr_len<32);
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) { //记录用了哪个规则进行匹配
          case TK_NOTYPE: //跳过空格
            break;
          case '+':
            tokens[nr_token++].type= '+';
            break;
          case '-':
            tokens[nr_token++].type = '-';
            break;
          case '*':
            tokens[nr_token++].type = '*';
            break;
          case '/':
            tokens[nr_token++].type = '/';
            break;
          case TK_LEFT_BRACKET:
            tokens[nr_token++].type = '(';
            break;
          case TK_RIGHT_BRACKET:
            tokens[nr_token++].type = ')';
            break;
          
          //need to record sub_str
          case TK_EQ:
            tokens[nr_token].type = TK_EQ;
            strncpy(tokens[nr_token++].str, substr_start,substr_len);
            break;
          case TK_NUM:
            tokens[nr_token].type = TK_NUM;
            strncpy(tokens[nr_token++].str, substr_start,substr_len);
            break;
          default: TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  for(int i = 0;i<nr_token;i++){
    if(tokens[i].type=='-'){
        if(i==0||(i!=0&&(tokens[i-1].type!=TK_NUM&&tokens[i-1].type!=')'))){
          printf("tokens[%d].type是负数\n",i);
          // tokens[i].type = TK_MINUS_SIGN;
        }
      }
  }
  return true;
}

uint32_t find_op(uint32_t p,uint32_t q){
  uint32_t min_op = -1;//记录优先级最低的符号
  for(uint32_t i = p;i<=q;i++){
    if(tokens[i].type=='('){ //main op 一定不在括号的表达式中
      while(tokens[i].type!=')')i++;
    }
    if(tokens[i].type=='*'||tokens[i].type=='/'){
        if(min_op==-1) min_op = i;
        if(tokens[min_op].type=='*'||tokens[min_op].type=='/'){// 查看上一个op的符号类型
          min_op = i;
        }        
    }
    if(tokens[i].type=='+'||tokens[i].type=='-'){
      min_op = i;
    }
  }
  return min_op;
}
bool check_parentheses(int p,int q){ //检查p和q包围的表达式是否
  if(tokens[p].type!='('||tokens[q].type!=')')return false;
  int l = p,r = q;
  while(l<r){
    if(tokens[l].type=='('){
      if(tokens[r].type==')'){
        l++,r--;
        continue;
      }
      else{
        r--;
      }
    }
    else if(tokens[r].type==')'){
      return false;
    }
    else l++;
  }
  return true;
}
word_t eval(uint32_t p,uint32_t q){  //p,q指示表达式的开始位置和结束位置
  if(p>q){
    assert(0);
    return -1;
  }
  else if(p==q){
    return atoi(tokens[p].str);
  }
  else if(check_parentheses(p,q)){ //如果p,q被对配对的括号包围
    return eval(p+1,q-1);
  }
  else{
    uint32_t op = find_op(p,q);
    word_t left_ans = eval(p,op-1);
    word_t right_ans = eval(op+1,q);

    switch(tokens[op].type){
      case '+':
        return left_ans+right_ans;
      break;
      case '-':
        return left_ans-right_ans;
      break;
      case '*':
        return left_ans*right_ans;
      break;
      case '/':
        assert(right_ans!=0);
        return left_ans/right_ans;
      break;
      default:
      assert(0);
      break;
    }
    
  }

}
word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    printf("make_token false\n");
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  return eval(0,nr_token-1);
}

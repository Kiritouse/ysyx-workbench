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
#define MaxSize 100
#define max(a, b)        ({                       \
                            typeof(a) _a = (a);   \
                            typeof(b) _b = (b);   \
                            (void) (&_a == &_b);  \
                            _a > _b ? _a : _b; })
typedef char ElemType;//定义栈元素类型
enum {
  TK_NOTYPE = 256, TK_EQ,TK_NEQ,
  TK_NUM,
  TK_LEFT_BRACKET,TK_RIGHT_BRACKET,//左右括号
  /* TODO: Add more token types */
  TK_NEGATIVE,//负数
  TK_AND,TK_OR,TK_NOT,TK_DEREF,//and or ! 指针解引用
  TK_REG,//寄存器
  TK_HEX,//十六进制
};
word_t vaddr_read(vaddr_t addr, int len);
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
  {"!=",TK_NEQ},
  {"&&",TK_AND},
  {"\\|\\|",TK_OR},
  //{"\\*",TK_DEREF},//指针解引用
  {"\\$(0|ra|sp|gp|tp|t[0-6]|s[0-9]|s1[0-1]|a[0-7])"},//riscv32 ,匹配寄存器,
  {"0x[0-9A-Fa-f]+",TK_HEX},//匹配16进制

};


#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */

//-------结构体定义部分------ //
typedef struct{
	ElemType data[MaxSize];
	int top;//指向栈顶元素的指针  
}MyStack;
//-------结构体定义部分------ //
 
//-------函数声明部分------ //
void InitStack(MyStack *S);//栈的初始化 
bool Push(MyStack *S,ElemType e);//将元素e压入栈 
bool Pop(MyStack *S,ElemType *x);//出栈
bool GetTop(MyStack *S,int *x);//获取当前栈顶元素 
bool StackEmpty(MyStack *S);//栈判空 
bool BraketCheck(char a[],int length);//括号匹配 


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

static Token tokens[1280] __attribute__((used)) = {}; //这个还是调大点,默认给的32
static int nr_token __attribute__((used))  = 0;


//

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
            strncpy(tokens[nr_token].str, substr_start,substr_len);
            *(tokens[nr_token++].str+substr_len)='\0';
            break;
          case TK_NEQ:
            tokens[nr_token].type = TK_NEQ;
            strncpy(tokens[nr_token].str, substr_start,substr_len);
            *(tokens[nr_token++].str+substr_len)='\0';
            break;
          case TK_AND:
            tokens[nr_token].type = TK_AND;
            strncpy(tokens[nr_token].str, substr_start,substr_len);
            *(tokens[nr_token++].str+substr_len)='\0';
            break;
          case TK_OR:
            tokens[nr_token].type = TK_OR;
            strncpy(tokens[nr_token].str, substr_start,substr_len);
            *(tokens[nr_token++].str+substr_len)='\0';
            break;
          case TK_NUM:
            tokens[nr_token].type = TK_NUM;
            strncpy(tokens[nr_token].str, substr_start,substr_len);
            *(tokens[nr_token++].str+substr_len)='\0';
            break;
          case TK_HEX:
            tokens[nr_token].type = TK_HEX;
            strncpy(tokens[nr_token].str,substr_start+2,substr_len-2);//丢掉0x
            *(tokens[nr_token++].str+substr_len)='\0';
            break;
          case TK_REG:
            tokens[nr_token].type = TK_REG;
            strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);//丢掉$
            *(tokens[nr_token++].str+substr_len)='\0';
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
  //处理一些特殊的符号
  for(int i = 0;i<nr_token;i++){ //处理负号
    if(tokens[i].type=='-'){
        if(i==0||(i!=0&&(tokens[i-1].type!=TK_NUM&&tokens[i-1].type!=')'))){
          tokens[i].type = TK_NEGATIVE;
        }
    }
    if(tokens[i].type=='*'){
      if(i<nr_token&&tokens[i+1].type == TK_HEX ){ //解引用地址
        tokens[i].type = TK_DEREF;
      }
    }
  }
  return true;
}

uint32_t find_op(int32_t p,int32_t q){ //TODO:
    int prior = 0; // 记录当前优先级
    int pos[20]  ={0};//优先级为i的运算符首次出现的下标，i越大，优先级越小
    for(int j = 0;j<20;j++)pos[j] = -1;
  MyStack S;
  InitStack(&S);
  for (int32_t i = q; i >= p; i--) { //之前这里用uint32_t,导致i--后溢出
    if (tokens[i].type == ')') {
      Push(&S, tokens[i].type); // 右括号入栈
    } else if (tokens[i].type == '(') {
      char topElem;
      if(StackEmpty(&S)){
        assert(0);
      }
      Pop(&S, &topElem); // 右括号出栈
    } else if (StackEmpty(&S)) { // 只在栈为空时检查运算符
       if(tokens[i].type==TK_NEGATIVE||tokens[i].type==TK_NOT||tokens[i].type==TK_DEREF){
        prior=max(prior,2);
        if(pos[2]==-1)pos[2]=i;
       }
     else  if (tokens[i].type == '*' || tokens[i].type == '/') {
        prior = max(prior, 3);
        if(pos[3]==-1)pos[3]=i;
      } else if (tokens[i].type == '+' || tokens[i].type == '-') {
        prior = max(prior,4);
        if(pos[4]==-1)pos[4]=i;
      }
      else if(tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ){
        prior = max(prior,7);
        if(pos[7]==-1)pos[7]=i;
      }
      else if(tokens[i].type == TK_AND){
        prior = max(prior,11);
        if(pos[11]==-1)pos[11]=i;
      }
      else if(tokens[i].type == TK_OR){
        prior = max(prior,12);
        if(pos[12]==-1)pos[12]=i;
      }
    }
  }
  return pos[prior];
}
/*
如果完整表达式被包含在一个括号里，我们就返回true，否则返回false
*/
static int check_parentheses(int p,int q){
//part1
  int cnt = 0;
  for(int i = p; i <= q;i ++)
  {
    if(tokens[i].type == '(')cnt ++;
    else if(tokens[i].type == ')')cnt --;
    
    if(cnt < 0)return -1;
  }
  if(cnt != 0) return -1;
  if(tokens[p].type != '(' || tokens[q].type != ')')return 0;
  int ret = check_parentheses(p+1,q-1);
  if(ret == -1)return 0;//说明此处的括号不能去掉
  else if(ret == 0||ret == 1)return 1;//说明此处的括号可以去掉
  else assert(0);
}
int32_t eval(int32_t p,int32_t q){  //p,q指示表达式的开始位置和结束位置
  if(p>q){
    assert(0);
    return -1;
  }
  else if(p==q){
    return atoi(tokens[p].str);
  }
  else if(check_parentheses(p,q)){
    return eval(p+1,q-1);
  }
  else{ //处理一些特殊的符号，例如负数，指针解引用
    if(p+1==q&&tokens[p].type==TK_NEGATIVE){
      return -atoi(tokens[q].str);
    }
    else if(tokens[p].type==TK_DEREF){ //p+1跟了一个16进制的地址
      int32_t addr = eval(p+1,q);
      printf("%x\n",addr);
      return vaddr_read(addr,4);
    }
  }

    int32_t op = find_op(p,q);
    int32_t left_ans = eval(p,op-1);

    int32_t right_ans = eval(op+1,q);
    

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
      case TK_EQ:
        return left_ans==right_ans;
      break;
      case TK_NEQ:
        return left_ans!=right_ans;
      break;
      case TK_AND:
        return left_ans&&right_ans;
      break;
      case TK_OR:
        return left_ans||right_ans;
      break;
      default:
      printf("No type is matched\n");
      assert(0);
      break;
    }
    


}
int32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    printf("make_token false\n");
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  return eval(0,nr_token-1);
}

//栈的初始化 
void InitStack(MyStack *S)
{
	//栈的初始化需要把栈顶指针赋值为-1,此时栈中元素个数为0 
	S->top = -1;
}
//入栈 
bool Push(MyStack *S,ElemType e)
{
	if(S->top == MaxSize -1) //如果栈满  也可以判断 length==MaxSize  
		return false;
	else 
		S->data[++S->top] = e;//指针先加一再将元素入栈  
	return true;
} 
//出栈
bool Pop(MyStack *S,ElemType *x)
{
	 
	if(S->top == -1)//栈空 
		return false;
	else
		*x = S->data[S->top--];//先获得元素，指针再执行--操作
	return true;
} 

//获取当前栈顶元素
bool GetTop(MyStack *S,int *x)
{
	if(S->top == -1)//如果栈空，则没有栈顶元素 
		return false;
	else
		*x = S->data[S->top];//获得栈顶元素 
	return true;
} 

//判断栈空 
bool StackEmpty(MyStack *S)
{
	return S->top==-1?true:false; 
}

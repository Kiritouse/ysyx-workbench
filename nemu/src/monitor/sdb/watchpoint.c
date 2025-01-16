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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  bool is_free;//某个监视点是否是空闲的 is_free = 1代表空闲
  char expr[100];
  int val;
} WP;

static WP wp_pool[NR_WP] = {}; //管理空闲的节点
static WP *head = NULL, *free_ = NULL;
static int CNT = 0;//记录被占用的监视点的index

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }
  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
bool new_wp(char*args){
  //从free_头部弹出一个空闲的监视点,并且将传入的表达式和val储存下来
  if(free_==NULL){
    printf("No free watchpoint free now\n");
    return false;
  }
  WP*new_one = free_;
  free_ = free_->next;
  new_one->NO = CNT++;
  new_one->next = NULL;
  strcpy(new_one->expr,args);
  bool success = true;
  new_one->val = expr(args,&success);
  if(success==false){
    printf("expr failed in new_wp\n");
    return false;
    assert(0);
  }
  //尾插进head
  if(head==NULL){
    head = new_one;
  }
  else{
    WP* p_head = head;
    while(p_head->next){
      p_head = p_head->next;
    }
    p_head->next = new_one;
  }
  printf("Watchpoint %d: %s\n", new_one->NO, new_one->expr);
  return true;
}
bool free_wp(int _NO){//将index为NO的从head链表中删除，并且添加到free链表中
  if(head==NULL){
    printf("No watchpoint\n");
    return false;
  }
  if(head->NO == _NO){
    head = head->next;
  }
  WP* p_head = head;
  WP* node = NULL;
  while(p_head->next){
      if(p_head->next->NO==_NO){
          node = p_head->next;
          p_head->next  = p_head->next->next;
          break;
      }
    p_head = p_head->next;
  }
  //在free链表头部添加节点
  if(node){
    node->next = free_;
    free_ = node;
    return true;
  }
  else{
    printf("free watchpoint false,there are no watchpoint busy\n");
    return false;
  }
}
void display_wp(){
  if(head==NULL){
    printf("no watchpoints\n");
    return;
  }
  WP* p_head = head;
  printf("%-8s%-8s\n", "No", "Expression");
  while (p_head) {
    printf("%-8d%-8s\n", p_head->NO, p_head->expr);
    p_head = p_head->next;
  }
}

void difftest_wp()
{
  if(head==NULL){
    printf("No Watchpoints\n");
  }
   WP* p_head = head;
  while (p_head) {
    bool success = true;
    word_t new = expr(p_head->expr, &success);
    if(!success){
      printf("expr error in difftest_wp\n");
      assert(0);
    }
    if (p_head->val != new) {
      printf("Watchpoint %d: %s\n"
        "Old value = %d\n"
        "New value = %d\n"
        , p_head->NO, p_head->expr, p_head->val, new);
      p_head->val = new;
      nemu_state.state=NEMU_STOP;
    }
    p_head = p_head->next;
  }
}



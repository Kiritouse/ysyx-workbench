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
  int new_val;
  int old_val;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

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
WP* new_wp(){//从free中返回空闲的监视点
  for(WP* p = free_;p->next!=NULL;p = p->next){
    if(p->is_free){
      p->is_free = 0;
      if(head==NULL){
        head = p;
      }
      return p;
    }
  }
  printf("No free watchpoint\n");
  assert(0);
  return NULL;
}
void free_wp(WP* wp){//将wp返回到free链表中
  if(head->NO==wp->NO){ //如果只有一个点
    head->is_free = 1;
    head = NULL;
    printf("all watchpoint are deleted\n");
    return;
  }
  for(WP* p = head;p->next!=NULL;p = p->next){
    if(p->next->NO == wp->NO){
      p->next = p->next->next; //因为是pool，所以不用删
      p->next->is_free = 1;
      printf("free one watchpoint success\n");
      return;
    }
  }
}
void watchpoint_display(){
  bool is_clear = true;
  for(int i = 0;i<NR_WP;i++){
    if(!wp_pool[i].is_free){
      printf("Watchpoint NO:%d, expr = %s, old_val = %d, new_val = %d\n",
      wp_pool[i].NO,wp_pool[i].expr,wp_pool[i].old_val,wp_pool[i].new_val);
      is_clear = false;
    }
  }
  if(is_clear){
    printf("No watchpoint is set\n");
  }
  return;
}
void watchpoint_delete(int NO){
  for(int i = 0;i<NR_WP;i++){
    if(wp_pool[i].NO==NO){
      free_wp(&wp_pool[i]);
      return;
    }
  }
}
void watchpoint_create(char*args){
  WP* new_p = new_wp();
  strcpy(new_p->expr,args);
  bool is_success= true;
  int _old = expr(args,&is_success);
  if(is_success){
    new_p->old_val = _old;
  }
  else{
    printf("watchpoint_create expr failed\n");
  }
  printf("watchpoint NO:%d success\n",new_p->NO);
}


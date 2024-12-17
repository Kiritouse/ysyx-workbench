#include "DLink_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
List* Createlist(){
    List* _list = calloc(1,sizeof(List));

    ListNode* head = calloc(1,sizeof(ListNode));
    ListNode* tail  = calloc(1,sizeof(ListNode));
    head->next = tail;
    tail->prev = head;
    _list->head = head;
    _list->tail = tail;
    return _list;
}

void add2Head(int value,List*list){
    ListNode*node = calloc(1,sizeof(ListNode));
    node->value = value;

    node->prev = list->head;
    node->next = list->head->next;
    list->head->next->prev = node;
    list->head->next = node;
    list->count++;                                                                                                                                                                                                   
}

void add2Tail(int value,List*list){
    ListNode*node = calloc(1,sizeof(ListNode));
    node->value = value;
    node->next = list->tail;
    node->prev = list->tail->prev;
    list->tail->prev->next = node;
    list->tail->prev = node;
    list->count++;
}

//这里建议最后再碰关于head或者Tail的指向
void removeHead(List*list){//移除链表首部节点
    ListNode*deleteNode = list->head->next;
    deleteNode->next->prev = list->head;//deletenode->next->prev = head
    list->head->next = deleteNode->next;  //head->next = deletenode->next
    //画图理解即可
    free(deleteNode);
    list->count--;
}

void removeTail(List*list){
    ListNode* deleteNode = list->tail->prev;
    deleteNode->prev->next = list->tail;
    list->tail->prev = deleteNode->prev;
    free(deleteNode);
    list->count--;
}

void displayList(List*list){
    ListNode*p = list->head;
    if(list->head->next==list->tail){
        printf("No data\n");
        return;
    }
    printf("list中有\n");
    while(p->next&&p->next!=list->tail){
        printf("%d ",p->next->value);
        p = p->next;
    }
    printf("\n");
}
void clearAllValueNode(List*list){
    if(!list)return;
    ListNode*p = list->head->next;
    ListNode*temp = NULL;
    while(p&&p!=list->tail){
        temp = p;
        p = p->next;
        free(temp);
    }
    //不要忘记还原更新head和tail
    list->head->next = list->tail;
    list->tail->prev = list->head;
    list->count = 0;
}
void clearList(List*list){
    if(list){ 
    clearAllValueNode(list);
    free(list->head);
    free(list->tail);
    free(list);
    }
}

//复制链表
List* copyList(List*list){
    List*new_list = Createlist();
    ListNode*p = list->head->next;
    while(p&&p!=list->tail){
        add2Tail(p->value,new_list);
        p = p->next;
    }
    return new_list;
}

//链接链表,将B链表链接到A链表尾部上
void linkList(List*A,List*B){
    A->tail->prev->next = B->head->next;
    B->head->next->prev = A->tail->prev;
    free(A->tail);
    A->tail = B->tail;
    A->count+=B->count;
    free(B->head);
    free(B);
}

//分割链表,从index处断开
List* splitList(List*list,int index){
    if(list<0||!list)return NULL;

    List*new_list = Createlist();
    ListNode*cur = list->head->next;
    int _cnt = 0;
    while(cur){
        if(_cnt==index){
            break;
        }
        cur = cur->next;
        _cnt++;
    }
    new_list->head->next = cur;
    new_list->tail->prev = list->tail->prev;
    new_list->count = list->count-index;

    list->tail->prev->next = new_list->tail;//先更新tail周边节点
    list->tail->prev = cur->prev;//再更新tail

    cur->prev->next = list->tail;
    cur->prev = new_list->head;
    
    return new_list;


}

int main(){
    List*list = Createlist();
    add2Head(1,list);
    add2Head(2,list);
    add2Head(3,list);
    displayList(list);

    removeHead(list);
    displayList(list);

    removeTail(list);
    displayList(list);

    clearAllValueNode(list);
    displayList(list);

    //连接测试
    add2Head(1,list);
    add2Head(2,list);
    add2Head(3,list);

    List*list2 = Createlist();
    add2Head(4,list2);
    add2Head(5,list2);
    add2Head(6,list2);

    linkList(list,list2);
    displayList(list);

    //分割测试
    List*list3 = splitList(list,3);
    if(list==NULL)printf("list==NULL\n");
    displayList(list);
    displayList(list3);


    clearList(list);
    clearList(list3);
    // clearList(list2);
}


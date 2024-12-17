#ifndef DLINK_LIST
#define DLINK_LIST
#include <stdlib.h>

struct ListNode;

typedef struct ListNode{
    struct ListNode *next;
    struct ListNode *prev;
    int value;
}ListNode;//用了typedef后，这里的东西就别忘了

typedef struct List{
    int count;
    ListNode*head;//伪节点
    ListNode*tail;
}List;

List* Createlist();
void add2Head(int value,List*list);
void add2Tail(int value,List*list);
void removeHead(List*list);
void removeTail(List*list);
void displayList(List*list);
void clearAllValueNode(List*list);
void clearList(List*list);
List* copyList(List*list);
void linkList(List*A,List*B);
List* splitList(List*list,int index);
#endif
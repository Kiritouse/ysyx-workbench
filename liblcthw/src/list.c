#include "lcthw/list.h"
#include "lcthw/dbg.h"
#include <stdio.h>
List *List_create()
{
    return calloc(1, sizeof(List));
}

void List_destroy(List *list)
{
    LIST_FOREACH(list, first, next, cur) {
        if(cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);
}


void List_clear(List *list)
{
    LIST_FOREACH(list, first, next, cur) {
        if(cur){
            if (cur->value) {
                printf("ready to do free: %p\n",cur->value);
                free(cur->value);
            }
        }
    }
}


void List_clear_destroy(List *list)
{
    List_clear(list);
    List_destroy(list);
}


void List_push(List *list, void *value)
{
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if(list->last == NULL) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;

error:
    return;
}

void *List_pop(List *list)
{
    ListNode *node = list->last;
    return node != NULL ? List_remove(list, node) : NULL;
}
//就是快速地向链表头部添加元素。由于找不到合适的词，这里我把它称为unshift。
void List_unshift(List *list, void *value)
{
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if(list->first == NULL) {
        list->first = node;
        list->last = node;
    } else {
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }

    list->count++;

error:
    return;
}

void *List_shift(List *list) //移除链表的首个元素并且返回首个元素指针
{
    ListNode *node = list->first;
    return node != NULL ? List_remove(list, node) : NULL;
}

void *List_remove(List *list, ListNode *node)
{
    void *result = NULL;

    check(list->first && list->last, "List is empty.");
    check(node, "node can't be NULL");

    if(node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
    } else if(node == list->first) {
        list->first = node->next;
        check(list->first != NULL, "Invalid list, somehow got a first that is NULL.");
        list->first->prev = NULL;
    } else if (node == list->last) {
        list->last = node->prev;
        check(list->last != NULL, "Invalid list, somehow got a next that is NULL.");
        list->last->next = NULL;
    } else {
        ListNode *after = node->next;
        ListNode *before = node->prev;
        after->prev = before;
        before->next = after;
    }

    list->count--;
    result = node->value;
    free(node);

error:
    return result;
}

//以下是附加题部分

List*List_copy(List*list){
    List*new_list= NULL;
    new_list = List_create();
    check_mem(new_list);
    LIST_FOREACH(list,first,next,cur){
        List_push(new_list,cur->value);
    }
    return new_list;
error:
    return NULL;
}

void List_join(List*list1,List*list2){ //list2连接到list1上
    if(list2->first==NULL) return;

    if (list1->last == NULL) {
        list1->first = list2->first;
        list1->last = list2->last;
    } else {
        list1->last->next = list2->first;
        list2->first->prev = list1->last;
        list1->last = list2->last;
    }

    list1->count += list2->count;

    // 清空 list2，但不销毁节点
    list2->first = NULL;
    list2->last = NULL;
    list2->count = 0;
}

List *List_split(List *list, int index)//返回从index处断开后的新的后面的链表
{
    if(!list)return NULL;
    check(index >= 0 && index < list->count, "Index out of range.");

    List *new_list = List_create();
    check_mem(new_list);

    if (index == 0) {
        // 如果索引为 0，直接将整个链表赋值给新链表，并清空原链表
        new_list->first = list->first;
        new_list->last = list->last;
        new_list->count = list->count;

        list->first = NULL;
        list->last = NULL;
        list->count = 0;

        return new_list;
    }

    ListNode *cur = list->first;
    for (int i = 0; i < index; i++) {
        cur = cur->next;
    }

    new_list->first = cur;
    new_list->last = list->last;
    new_list->count = list->count - index;

    list->last = cur->prev;
    list->last->next = NULL;
    cur->prev = NULL;
    list->count = index;

    return new_list;

error:
    if (new_list) List_destroy(new_list);
    return NULL;
}
// int main(){
//     List*list = List_create();
//     List*list2 = List_create();
//     int *value1 = malloc(sizeof(int));
//     int *value2 = malloc(sizeof(int));
//     *value1 = 1;
//     *value2 = 2;
//     int *value3 = malloc(sizeof(int));
//     int *value4 = malloc(sizeof(int));
//     *value3 = 3;
//     *value4 = 4;
//     List_push(list,value1);
//     List_push(list,value2);
//     List_push(list2,value3);
//     List_push(list2,value4);
//     List_join(list,list2);//注意Join的时候，两个链表的每个数据一定要在不同的内存中，不然会出现重复删除的情况
//    List*list3 = List_split(list,2);
//     // LIST_FOREACH(list,first,next,cur){
//     //     printf("%d",*(int*)cur->value);
//     // }
//     LIST_FOREACH(list3,first,next,cur){
//         printf("%d",*(int*)cur->value);
//     }
//     printf("\n");
//     List_clear_destroy(list);
//     List_clear_destroy(list2);
// }
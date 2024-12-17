
#include "lcthw/list_algos.h"
//#define SORT_DEBUG
inline void ListNode_swap(ListNode*node1,ListNode*node2){
    if (node1 == NULL || node2 == NULL) return;
    void*temp = node1->value;
    node1->value = node2->value;
    node2->value = temp;
}

// int List_bubble_sort(List*list,List_compare cmp){ //冒泡排序测试通过
//     int ret = 0;//代表排序是否成功
//     if(List_count(list)<=1){
//         return 0;
//     }
//     for(int i = 0;i<List_count(list)-1;i++){
//         ListNode*cur = list->first;
//         for(int j = 0;j<List_count(list)-1-i;j++){ 
//             if(cur->next){
//                  if(cmp(cur->value,cur->next->value)>0){
//                        ListNode_swap(cur,cur->next);
//                  }
//                  cur = cur->next;   
//             }
//         }
//     }
//     ret = 1;
//     return ret;
// }
int List_bubble_sort(List *list, List_compare cmp)
{
    if (list == NULL || cmp == NULL) return -1;
    int sorted = 1;
    if(List_count(list) <= 1) {
        return 0;  // already sorted
    }

    do {
        sorted = 1;
        LIST_FOREACH(list, first, next, cur) { //直接暴力遍历，每次都把大的放后面即可
            if(cur->next) {
                if(cmp(cur->value, cur->next->value) > 0) {
                    ListNode_swap(cur, cur->next);
                    sorted = 0;
                }
            }
        }
    } while(!sorted);
#ifdef SORT_DEBUG
    if (!is_sorted(list, cmp)) {
        printf("List_bubble_sort failed\n");
        return -1;
    }
#endif
    return 0;
}
inline List *List_merge(List *left, List *right, List_compare cmp)
{
    List *result = List_create();//临时储存合并后的链表,wiki上的数组代码似乎有问题，会存在temp被覆盖的情况？竟然没人维护
    void *val = NULL;

    while(List_count(left) > 0 || List_count(right) > 0) {
        if(List_count(left) > 0 && List_count(right) > 0) {
            if(cmp(List_first(left), List_first(right)) <= 0) {//两边都有元素则一一比较
                val = List_shift(left);
            } else {
                val = List_shift(right);
            }
            List_push(result, val);
        } else if(List_count(left) > 0) {
            List_join(result,left);
            // val = List_shift(left);
            // List_push(result, val);
        } else if(List_count(right) > 0) {
            List_join(result,right);
            // val = List_shift(right);
            // List_push(result, val);
        }
    }

    return result;
}

List *List_merge_sort(List *list, List_compare cmp)
{
    if(List_count(list) <= 1) {
        return list;
    }

    // List *left = List_create();
    // List *right = List_create();
    //使用List_split函数代替分割
    List* right = List_split(list,List_count(list)/2);//分割成左右两个部分
    int middle = List_count(list) / 2;

    // LIST_FOREACH(list, first, next, cur) {  //分割成左右两个部分
    //     if(middle > 0) {
    //         List_push(left, cur->value);
    //     } else {
    //         List_push(right, cur->value);
    //     }

    //     middle--;
    // }

    List *sort_left = List_merge_sort(list, cmp);//左边部分再进行分割
    List *sort_right = List_merge_sort(right, cmp);//右边部分再进行分割

    // if(sort_left != left) List_destroy(left);//销毁未排序的部分
    // if(sort_right != right) List_destroy(right);//销毁原始的未排序部分

    return List_merge(sort_left, sort_right, cmp);//再合并
}

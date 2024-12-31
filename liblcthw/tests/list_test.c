#include "minunit.h"
#include "../src/lcthw/list.h"
#include <assert.h>

static List *list = NULL;
char *test1 = "test1 data";
char *test2 = "test2 data";
char *test3 = "test3 data";

static List*list2 = NULL;
char *test2_1 = "test2_1 data";
char *test2_2 = "test2_2 data";
char *test2_3 = "test2_3 data";

static List*list3 = NULL;
char *test3_1 = "test3_1 data";
char *test3_2 = "test3_2 data";
char *test3_3 = "test3_3 data";

static List*original_list = NULL;
static List*copyied_list = NULL;
char *testcpy_1 = "tescpy_1 data";
char *testcpy_2 = "tescpy_2 data";
char *test_create()
{
    list = List_create();
    mu_assert(list != NULL, "Failed to create list.");
    return NULL;
}


char *test_destroy()
{
    if(list)
    List_clear_destroy(list);
    return NULL;

}


char *test_push_pop()
{
    List_push(list, test1);
    mu_assert(List_last(list) == test1, "Wrong last value.");

    List_push(list, test2);
    mu_assert(List_last(list) == test2, "Wrong last value");

    List_push(list, test3);
    mu_assert(List_last(list) == test3, "Wrong last value.");
    mu_assert(List_count(list) == 3, "Wrong count on push.");

    char *val = List_pop(list);
    mu_assert(val == test3, "Wrong value on pop.");

    val = List_pop(list);
    mu_assert(val == test2, "Wrong value on pop.");

    val = List_pop(list);
    mu_assert(val == test1, "Wrong value on pop.");
    mu_assert(List_count(list) == 0, "Wrong count after pop.");

    return NULL;
}

char *test_unshift()
{
    List_unshift(list, test1);
    mu_assert(List_first(list) == test1, "Wrong first value.");

    List_unshift(list, test2);
    mu_assert(List_first(list) == test2, "Wrong first value");

    List_unshift(list, test3);
    mu_assert(List_first(list) == test3, "Wrong last value.");
    mu_assert(List_count(list) == 3, "Wrong count on unshift.");

    return NULL;
}

char *test_remove()
{
    // we only need to test the middle remove case since push/shift
    // already tests the other cases

    char *val = List_remove(list, list->first->next);
    mu_assert(val == test2, "Wrong removed element.");
    mu_assert(List_count(list) == 2, "Wrong count after remove.");
    mu_assert(List_first(list) == test3, "Wrong first after remove.");
    mu_assert(List_last(list) == test1, "Wrong last after remove.");

    return NULL;
}


char *test_shift()
{
    mu_assert(List_count(list) != 0, "Wrong count before shift.");

    char *val = List_shift(list);
    mu_assert(val == test3, "Wrong value on shift.");

    val = List_shift(list);
    mu_assert(val == test1, "Wrong value on shift.");
    mu_assert(List_count(list) == 0, "Wrong count after shift.");

    return NULL;
}

char*test_copy(){
    original_list = List_create();
    copyied_list = List_create();
    List_push(original_list,testcpy_1);
    List_push(original_list,testcpy_2);
    copyied_list = List_copy(original_list); 
    mu_assert(copyied_list != NULL, "Failed to copy list.");
    mu_assert(List_count(copyied_list) == List_count(original_list), "Copied list has wrong count.");

    ListNode *cur1 = original_list->first;
    ListNode *cur2 = copyied_list->first;

    while (cur1 != NULL && cur2 != NULL) {
        mu_assert(cur1->value == cur2->value, "Copied list has wrong values.");//注意value是一个地址，这两个链表的数据地址是一样的
        cur1 = cur1->next;
        cur2 = cur2->next;
    }

    mu_assert(cur1 == NULL && cur2 == NULL, "Copied list has different length.");
    return NULL;
}

char *test_join()
{
    list2 = List_create();
    list3 = List_create();
    mu_assert(list2 != NULL, "Failed to create list2.");
    mu_assert(list2 != NULL, "Failed to create list3.");
    List_push(list2, test2_1);
    List_push(list2, test2_2);
    List_push(list2, test2_3);

    List_push(list3, test3_1);
    List_push(list3, test3_2);
    List_push(list3, test3_3);

    List_join(list2, list3);
    mu_assert(List_count(list2) == 6, "Joined list has wrong count.");
    mu_assert(List_count(list3) == 0, "List2 should be empty after join.");

    ListNode *cur = list2->first;
    mu_assert(cur->value == test2_1, "Wrong value in joined list.");
    cur = cur->next;
    mu_assert(cur->value == test2_2, "Wrong value in joined list.");
    cur = cur->next;
    mu_assert(cur->value == test2_3, "Wrong value in joined list.");
    cur = cur->next;
    mu_assert(cur->value == test3_1, "Wrong value in joined list.");
    cur = cur->next;
    mu_assert(cur->value == test3_2, "Wrong value in joined list.");
    cur = cur->next;
    mu_assert(cur->value == test3_3, "Wrong value in joined list.");

    return NULL;
}

char *test_split()
{
    list3 = List_split(list2, 2);
    mu_assert(list2 != NULL, "Failed to split list.");
    mu_assert(List_count(list2) == 2, "Original list has wrong count after split.");
    mu_assert(List_count(list3) == 4, "New list has wrong count after split.");

    ListNode *cur1 = list2->first;
    ListNode *cur2 = list3->first;

    mu_assert(cur1->value == test2_1, "Wrong value in original list after split.");
    cur1 = cur1->next;
    mu_assert(cur1->value == test2_2, "Wrong value in original list after split.");

    mu_assert(cur2->value == test2_3, "Wrong value in new list after split.");
    cur2 = cur2->next;
    mu_assert(cur2->value == test3_1, "Wrong value in new list after split.");
    cur2 = cur2->next;
    mu_assert(cur2->value == test3_2, "Wrong value in new list after split.");
    cur2 = cur2->next;
    mu_assert(cur2->value == test3_3, "Wrong value in new list after split.");
    return NULL;
}



char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create);//创建一个空的list
    mu_run_test(test_push_pop);//push三个数据进list（尾插）,再弹出三个数据，此时list为空
    mu_run_test(test_unshift);//头依次插入test1,test2,test3  ，此时list中的数据应该是test3,test2,test1
    mu_run_test(test_remove);//移除test2，此时list中的数据应该是test3,test1
    mu_run_test(test_shift);//移除链表的首个元素，这个函数此时操作后list中为空
    mu_run_test(test_copy);
    mu_run_test(test_join);//创建list2和list3,并且各自移入3个数据，然后将list3连接到list2中，之后list3中的数据就为空了
    mu_run_test(test_split);//将list2从index2后的分割给list3，此时list2中的数据应该是test2_1,test2_2，list3中的数据应该是test2_3,test3_1,test3_2,test3_3
    mu_run_test(test_destroy);
    

    return NULL;
}

RUN_TESTS(all_tests);
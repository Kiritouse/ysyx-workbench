#include "minunit.h"
#include <lcthw/list_algos.h>
#include <assert.h>
#include <string.h>
#include "../src/lcthw/list.h"
#include <time.h>

char *values[] = {"XXXX", "1234", "abcd", "xjvef", "NDSS"};
#define NUM_VALUES 5
#define NUM_ITERATIONS 100000
List *create_words()
{
    int i = 0;
    List *words = List_create();

    for(i = 0; i < NUM_VALUES; i++) {
        List_push(words, values[i]);
    }

    return words;
}

int is_sorted(List *words)
{
    LIST_FOREACH(words, first, next, cur) {
        if(cur->next && strcmp(cur->value, cur->next->value) > 0) {
            debug("%s %s", (char *)cur->value, (char *)cur->next->value);
            return 0;
        }
    }

    return 1;
}

char *test_bubble_sort()
{
    List *words = create_words();
 
    // should work on a list that needs sorting
    int rc = List_bubble_sort(words, (List_compare)strcmp);
    mu_assert(rc == 0, "Bubble sort failed.");
    mu_assert(is_sorted(words), "Words are not sorted after bubble sort.");

    // should work on an already sorted list
    rc = List_bubble_sort(words, (List_compare)strcmp);
    mu_assert(rc == 0, "Bubble sort of already sorted failed.");
    mu_assert(is_sorted(words), "Words should be sort if already bubble sorted.");

    List_destroy(words);

    // should work on an empty list
    words = List_create(words);
    rc = List_bubble_sort(words, (List_compare)strcmp);
    mu_assert(rc == 0, "Bubble sort failed on empty list.");
    mu_assert(is_sorted(words), "Words should be sorted if empty.");

    List_destroy(words);

    return NULL;
}

char *test_merge_sort()
{
    List *words = create_words();

    // should work on a list that needs sorting
    
    List *res = List_merge_sort(words, (List_compare)strcmp);
    mu_assert(is_sorted(res), "Words are not sorted after merge sort.");
    List *res2 = List_merge_sort(res, (List_compare)strcmp);
    mu_assert(is_sorted(res), "Should still be sorted after merge sort.");
    List_destroy(res2);
    List_destroy(res);

    List_destroy(words);
    return NULL;
}

char* test_measure_time(){
    clock_t start,end;
    double cpu_time_used_bubble;
    List*words;
    start = clock();
    for(int i = 0;i<NUM_ITERATIONS;i++){
        words = create_words();
        List_bubble_sort(words, (List_compare)strcmp);
        end = clock();
        cpu_time_used_bubble = ((double)(end-start))/CLOCKS_PER_SEC;
        List_destroy(words);
    }
    printf("bubble sort time:%f\n",cpu_time_used_bubble);
    double cpu_time_used_merge;
    start = clock();
    for(int i = 0;i<NUM_ITERATIONS;i++){
        words = create_words();
        List*res = List_merge_sort(words, (List_compare)strcmp);
        end = clock();
        cpu_time_used_merge = ((double)(end-start))/CLOCKS_PER_SEC;
        List_destroy(res);
        List_destroy(words);
    }
    printf("merge sort time:%f\n",cpu_time_used_merge);
    return NULL;
}
char *test_insert_sorted()
{
    List *words = List_create();
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    for (int i = 0; i < NUM_VALUES; i++) {
        List_insert_sorted(words, values[i], (List_compare)strcmp);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    //mu_assert(test_is_sorted(words), "Words are not sorted after insert sorted.");
    List_destroy(words);

    printf("Time taken for List_insert_sorted: %f seconds\n", cpu_time_used);

    return NULL;
}

char *test_create_and_sort()
{
    List *words;
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    words = create_words();
    List_bubble_sort(words, (List_compare)strcmp);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    //mu_assert(test_is_sorted(words), "Words are not sorted after bubble sort.");
    List_destroy(words);

    printf("Time taken for create and sort: %f seconds\n", cpu_time_used);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_bubble_sort);
    mu_run_test(test_merge_sort);
    mu_run_test(test_measure_time);
    mu_run_test(test_insert_sorted);
    mu_run_test(test_create_and_sort);
    return NULL;
}

RUN_TESTS(all_tests);
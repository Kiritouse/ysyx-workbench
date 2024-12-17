#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;

    // go through each string in argv
    // why am I skipping argv[0]?
    for(i = 0; i < argc; i++) {//从0开始也是正确的，会打印出程序名称
        printf("arg %d: %s\n", i, argv[i]);
    }

    // let's make our own array of strings
    char *states[] = {
        "California", "Oregon",
        "Washington", "Texas"
    };
    int num_states = 4;

    for(i = 0; i < num_states; i++) {
        printf("state %d: %s\n", i, states[i]);
    }

     // 将 states 的一个元素赋值给 argv 中的元素
    if (argc > 1) {
        states[1] = argv[1];
        printf("After assignment, state 1: %s\n", states[1]);
    }

    // 将 argv 中的一个元素赋值给 states 的一个元素
    if (argc > 2) {
        argv[2] = states[2];
        printf("After assignment, arg 2: %s\n", argv[2]);
    }

    return 0;
}
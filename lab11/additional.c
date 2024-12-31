#include <stdio.h>

int main(int argc, char *argv[])
{
    // 倒序遍历 argv
    int i = argc - 1;
    while (i >= 0) {
        printf("arg %d: %s\n", i, argv[i]);
        i--;
    }

    // 创建自己的字符串数组
    char *states[] = {
        "California", "Oregon",
        "Washington", "Texas"
    };

    int num_states = 4;

    // 倒序遍历 states
    i = num_states - 1;
    while (i >= 0) {
        printf("state %d: %s\n", i, states[i]);
        i--;
    }

    // 使用 while 循环将 argv 中的值复制到 states
    i = 0;
    while (i < argc && i < num_states) {
        states[i] = argv[i];
        i++;
    }

    // 打印复制后的 states
    i = 0;
    while (i < num_states) {
        printf("copied state %d: %s\n", i, states[i]);
        i++;
    }

    return 0;
}
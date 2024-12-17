#include <stdio.h>

int main(int argc, char *argv[])
{
    int distance = 100;
    float power = 2.345f;
    double super_power = 56789.4532;
    char initial = 'A';
    char first_name[] = "Zed";
    char last_name[] = "Shaw";

    printf("You are %d miles away.\n", distance);
    printf("You have %f levels of power.\n", power);
    printf("You have %f awesome super powers.\n", super_power);
    printf("I have an initial %c.\n", initial);
    printf("I have a first name %s.\n", first_name);
    printf("I have a last name %s.\n", last_name);
    printf("My whole name is %s %c. %s.\n",
            first_name,initial, last_name);//Invalid read of size 1,访问无效的内存地址，因为%s期望读取一个字符串，但是这里的initial只有一个字符

    //以下是附加题的尝试
    char *test = NULL;
    printf("You are %s miles away.\n", test);

    // 使用高级的占位符
    printf("Distance in octal: %o\n", distance); // 以八进制打印
    printf("Distance in hexadecimal: %x\n", distance); // 以十六进制打印
    printf("Distance in hexadecimal (uppercase): %X\n", distance); // 以大写十六进制打印

        // 打印空字符串
    printf("Empty string: \"%s\"\n", ""); // 打印空字符串
    return 0;
}
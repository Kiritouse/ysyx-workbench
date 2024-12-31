#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("ERROR: You need at least one argument.\n");
        return 1;
    }

    for(int j = 1; j < argc; j++) {
        for(int i = 0, letter = argv[j][i]; argv[j][i] != '\0'; i++, letter = argv[j][i]) {
            if(letter >= 65 && letter <= 90) letter += 32;

            if(letter == 'a') {
                printf("%d: 'a'\n", i);
            } else if(letter == 'e') {
                printf("%d: 'e'\n", i);
            } else if(letter == 'i') {
                printf("%d: 'i'\n", i);
            } else if(letter == 'o') {
                printf("%d: 'o'\n", i);
            } else if(letter == 'u') {
                printf("%d: 'u'\n", i);
            } else if(letter == 'y') {
                if(i > 2) {
                    printf("%d: 'y'\n", i);
                }
                break; // 这里的break会跳出当前的for循环
            } else {
                printf("%d: %c is not a vowel\n", i, letter);
            }
        }
    }

    return 0;
}
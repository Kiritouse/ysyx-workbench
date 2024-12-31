#include <stdio.h>
void print_using_indexing(int *ages, char **names, int count) {
    for(int i = 0; i < count; i++) {
        printf("%s has %d years alive.\n", names[i], ages[i]);
    }
}

void print_using_pointers(int *ages, char **names, int count) {
    for(int i = 0; i < count; i++) {
        printf("%s is %d years old.\n", *(names + i), *(ages + i));
    }
}

void print_using_pointers_as_arrays(int *ages, char **names, int count) {
    for(int i = 0; i < count; i++) {
        printf("%s is %d years old again.\n", names[i], ages[i]);
    }
}

void print_using_complex_pointers(int *ages, char **names, int count) {
    int *cur_age = ages;
    char **cur_name = names;
    while((cur_age - ages) < count) {
        printf("%s lived %d years so far.\n", *cur_name, *cur_age);
        cur_name++;
        cur_age++;
    }
}

void print_addresses(int *ages, char **names, int count) {
    for(int i = 0; i < count; i++) {
        printf("Address of %s: %p, Address of age: %p\n", names[i], (void*)&names[i], (void*)&ages[i]);
    }
}
int main(int argc, char *argv[])
{
    // create two arrays we care about
    int ages[] = {23, 43, 12, 89, 2};
    char *names[] = {
        "Alan", "Frank",
        "Mary", "John", "Lisa"
    };

    // safely get the size of ages
    int count = sizeof(ages) / sizeof(int);

    // first way using indexing
    print_using_indexing(ages, names, count);

    printf("---\n");

    // second way using pointers
    print_using_pointers(ages, names, count);

    printf("---\n");

    // third way, pointers are just arrays
    print_using_pointers_as_arrays(ages, names, count);

    printf("---\n");

    // fourth way with pointers in a stupid complex way
    print_using_complex_pointers(ages, names, count);

    printf("---\n");

    // print addresses
    print_addresses(ages, names, count);

    return 0;
}
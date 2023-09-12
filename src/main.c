#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

void assign_and_print(int *ptr, int n)
{
    for (int i = 0; i < n; i++)
    {
        ptr[i] = i;
    }
    
    for (int i = 0; i < n; i++)
    {
        printf("[%d] ", ptr[i]);
    }
    printf("\n");
    return;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <number of integers>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int *ptr = mymalloc(n * sizeof(int));

    assign_and_print(ptr, n);

    ptr = myrealloc(ptr, 2 * n * sizeof(int));
    
    assign_and_print(ptr, 2 * n);

    myfree(ptr);
    // check if myfree worked.
    ptr = NULL;

    return 0;
}

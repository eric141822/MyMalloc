#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <number of integers>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int *ptr = mymalloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
    {
        ptr[i] = i;
    }
    
    for (int i = 0; i < n; i++)
    {
        printf("%d\n", ptr[i]);
    }
    myfree(ptr);
    // check if myfree worked.
    ptr = NULL;

    return 0;
}

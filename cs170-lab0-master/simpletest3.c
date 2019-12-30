#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "my_malloc.h"

int main(int argc, char *argv[])
{
    InitMyMalloc();
    char *a1;
    char *a2;
    /*
    * must be first call in the program
    */
    InitMyMalloc();

    a1 = (char *)MyMalloc(128);
    if(a1 == NULL)
    {
        fprintf(stderr,"call to MyMalloc(128) failed\n");
        fflush(stderr);
        exit(1);
    }

    PrintMyMallocFreeList();

    a1 = (char *)MyMalloc(13);

    // PrintMyMallocFreeList();

    // a2 = (char *)MyMalloc(16777216);
    // if (a2 == NULL) {
    //     fprintf(stderr,"call to MyMalloc() failed successfully\n");
    // }

    return(0);
}
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "my_malloc.h"

int main(int argc, char *argv[])
{
	char *a1;
	char *a2;
	char *a3;
	char *a4;

	/*
	 * must be first call in the program
	 */
	InitMyMalloc();

	printf("\nMYMALLOC(128)\n");
	a1 = (char *)MyMalloc(128);
	if(a1 == NULL)
	{
		fprintf(stderr,"call to MyMalloc(128) failed\n");
		fflush(stderr);
		exit(1);
	}

	PrintMyMallocFreeList();

	printf("\nMYFREE(128)\n");
	MyFree(a1);

	PrintMyMallocFreeList();

	printf("\nMYMALLOC(32)\n");
	a2 = (char *)MyMalloc(32);
	if(a2 == NULL)
	{
		fprintf(stderr,"first call to MyMalloc(32) failed\n");
		fflush(stderr);
		exit(1);
	}

	PrintMyMallocFreeList();

	printf("\nMYMALLOC(104)\n");
	a3 = (char *)MyMalloc(104);
	if(a3 == NULL)
	{
		fprintf(stderr,"call to MyMalloc(104) failed\n");
		fflush(stderr);
		exit(1);
	}

	PrintMyMallocFreeList();

	printf("\nMYMALLOC(8)\n");
	a4 = (char *)MyMalloc(8);
	if(a4 == NULL)
	{
		fprintf(stderr,"call to MyMalloc(8) failed\n");
		fflush(stderr);
		exit(1);
	}

	PrintMyMallocFreeList();

	printf("\nMYFREE(32)\n");
	MyFree(a2);
	PrintMyMallocFreeList();

	printf("\nMYFREE(8)\n");
	MyFree(a4);
	PrintMyMallocFreeList();
	
	printf("\nMYMALLOC(104)\n");
	MyFree(a3);
	PrintMyMallocFreeList();

	printf("ENDING MYFREELIST\n");
	PrintMyMallocFreeList();

	return(0);
}

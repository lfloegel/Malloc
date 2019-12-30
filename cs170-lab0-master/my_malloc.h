#if !defined(MY_MALLOC_H)
#define MY_MALLOC_H

#define MAX_MALLOC_SIZE (1024*1024*16)

struct malloc_stc { // a block of memory referencing the next and prev blocks.
    struct malloc_stc *next;
    struct malloc_stc *prev;
    int size;
    unsigned char *buffer; // buffer: a region of physical memory storage used to temporarily store data while it is being moved from one place to another. unsigned char is a byte value from 0-255, not a character.
};


// Global variables
static struct malloc_stc *MyFreeList; // equivalent to a list's HEAD node
static unsigned char MyBuff[MAX_MALLOC_SIZE]; // buffer must always be of type unsigned char

void InitMyMalloc();
void *MyMalloc(int size);
void MyFree(void *buffer);

void PrintMyMallocFreeList();		/* optional for debugging */


#endif
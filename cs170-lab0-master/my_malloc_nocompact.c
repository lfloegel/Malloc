#include "my_malloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void InitMyMalloc() {
    MyFreeList = (struct malloc_stc *)MyBuff; // Casting the big boi buffer to a malloc_stc type (the head's bookkeeper)
    MyFreeList -> next = NULL;
    MyFreeList -> prev = NULL;
    MyFreeList -> size = sizeof(MyBuff) - sizeof(struct malloc_stc); // Size is the size of free space available to be allocated.
    MyFreeList -> buffer = (unsigned char *)(MyBuff + sizeof(struct malloc_stc)); // buffer points to the first byte of storage in the buffer usable by the caller of MyMalloc()
    //takes the address of MyBuff (at the beginning of the list) and adds 32 to it
}

// test cases: size = 32, 10, -5, 16777216
// cases: when no more space in buffer is left (MyFreeList = NULL)
void *MyMalloc(int size) {

    if (size <= 0 || size > MAX_MALLOC_SIZE) return NULL;
    
    // Rounding up to 8
    if (size % 8 != 0) {
        size = size + 8 - (size % 8);
    }

    //ALLOCATING BLOCK
    //setting a temp pointer to MyFreeList to return buffer of allocated block to user
    struct malloc_stc* temp_ptr = MyFreeList;
    struct malloc_stc* result =  NULL;
    bool isIter = true;
    while (isIter && temp_ptr) {
        if ((temp_ptr -> size == sizeof(struct malloc_stc) + size) || 
        ((temp_ptr -> size >= size) && (temp_ptr -> size < (size + sizeof(struct malloc_stc))))) { // if allocation fills entire free block and we should not split it:
            printf("Hijacking...\n");
            // REMOVING ENTIRE FREE BLOCK FROM MYFREELIST
            struct malloc_stc* pre = temp_ptr -> prev;
            if (temp_ptr -> next) temp_ptr -> next -> prev = pre;
            if (temp_ptr -> prev) {
                temp_ptr -> prev -> next = temp_ptr -> next;
            } else {
                MyFreeList = temp_ptr -> next; // if we're deallocating head of MFL
            }
            
            // SETTING ALLOCATED BLOCK
            temp_ptr -> next = NULL;
            temp_ptr -> prev = NULL;
            temp_ptr -> buffer = ((unsigned char *)temp_ptr) + sizeof(struct malloc_stc);
            result = temp_ptr;
            isIter = false;
        } else if (temp_ptr -> size > sizeof(struct malloc_stc) + size) { // non-hijacking, splitting case
            printf("Splitting...\n");
            int size_temp = temp_ptr -> size;

            // KEEPING TEMP->NEXT
            struct malloc_stc* tempNext = temp_ptr -> next;
            struct malloc_stc* tempPrev = temp_ptr -> prev;
            // SETTING ALLOCATED BLOCK
            temp_ptr -> next = NULL;
            temp_ptr -> prev = NULL;
            temp_ptr -> size = size;
            temp_ptr -> buffer = ((unsigned char *)temp_ptr) + sizeof(struct malloc_stc);
            result = temp_ptr;

            // SPLITTING FREE BLOCK
            unsigned char* newBlockAddress = ((unsigned char *)temp_ptr) + sizeof(struct malloc_stc) + size; //good
            struct malloc_stc* freed = (struct malloc_stc *)newBlockAddress; //good

            // SETTING FREE BLOCK'S NEW ADDRESS
            freed -> next = tempNext;
            freed -> prev = tempPrev;
            if (tempNext) {
                tempNext -> prev = freed;
            }
            if (tempPrev) {
                tempPrev -> next = freed;
            }

            freed -> size = size_temp - size - (sizeof(struct malloc_stc)); //good
            freed -> buffer = (((unsigned char*)temp_ptr) + sizeof(struct malloc_stc)); //good
 
            if (MyFreeList == result) { // if we need to reassign head of MFL
                MyFreeList = freed;
            }           

            isIter = false;
        }
        if (!isIter) {
            printf("ALLOCATED BLOCK\n");
            printf("block: %p\n", temp_ptr);
            printf("size: %d\n", temp_ptr -> size);
            printf("next: %p\n", temp_ptr -> next);
            printf("prev: %p\n", temp_ptr -> prev);
            printf("buffer: %p\n", temp_ptr -> buffer);
        }
        temp_ptr = temp_ptr -> next;
    }
    
    if (!result) return result; // need to handle case where no free blocks can be used.
    return result -> buffer;
}

void MyFree(void *buffer) {

    printf("MYFREE WAS CALLED\n");
    struct malloc_stc* toFree = (struct malloc_stc*)((unsigned char*)buffer - sizeof(struct malloc_stc)); // points to front of bookkeeper ahead of buffer
    // if address is before MyFreeList
    
    if (!MyFreeList) {
        MyFreeList = toFree;
        toFree -> prev = NULL;
        toFree -> next = NULL;
    } else if (toFree < MyFreeList) {
        MyFreeList -> prev = toFree;
        toFree -> next = MyFreeList;
        MyFreeList = toFree;
        MyFreeList -> prev = NULL;
    } else { // if address is after MyFreeList -- "insertion sort"
        struct malloc_stc* curr = MyFreeList;
        struct malloc_stc* ahead = MyFreeList -> next;
        bool hasFound = false;
        while (ahead && !hasFound) { // finding where toFree belongs between two free list blocks
            if (toFree < ahead && toFree > curr) {
                curr -> next = toFree;
                ahead -> prev = toFree;
                toFree -> next = ahead;
                toFree -> prev = curr;
                hasFound = true;
            }
            curr = curr -> next;
            ahead = ahead -> next;
        }

        if (!hasFound) { // if we've reached the end of MyFreeList
            curr -> next = toFree;
            toFree -> prev = curr;
            toFree -> next = NULL;
        }
        
    }
}
/* optional for debugging */
void PrintMyMallocFreeList() {
    struct malloc_stc * ptr = MyFreeList;
    printf("***PRINTING FREE LIST***");
    while (ptr) {
        printf("\nblock: %p\n", ptr);
        printf("size: %d\n", ptr -> size);
        printf("next: %p\n", ptr -> next);
        printf("prev: %p\n", ptr -> prev);
        printf("buffer: %p\n", ptr -> buffer);
        ptr = ptr -> next;
    }
}		
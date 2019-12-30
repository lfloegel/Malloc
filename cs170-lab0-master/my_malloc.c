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
}

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
            freed -> buffer = (((unsigned char*)freed) + sizeof(struct malloc_stc)); //good
 
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
    if (!buffer) {
        return; // Returns if InitMyMalloc() has not been called yet
    }
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

    printf("GETS TO COALESCING\n");
    // COALESCING:
        bool hasCoalescedPrev = false;
        struct malloc_stc* size_ptr = toFree->prev;
        if (toFree -> prev) {
            printf("**COALESCE LEFT**\n");
            if ((unsigned char*)toFree == (unsigned char*)(toFree -> prev) + sizeof(struct malloc_stc) + (toFree -> prev -> size)) {
                printf("**INSIDE OF COALESCING**\n");
                toFree -> prev -> size += toFree -> size + sizeof(struct malloc_stc);
                toFree -> size = toFree -> prev -> size;

                struct malloc_stc* tempPrev = toFree -> prev;
                tempPrev -> next = toFree -> next; //fine
                if(toFree->next) {
                    toFree -> next -> prev = tempPrev;
                } 
                toFree -> prev = NULL;

                hasCoalescedPrev = true;
            }
        }
        // Will only go here if toFree -> prev == NULL as toFree -> next will be equal to NULL, toFree is at the beginning of the list
        if (toFree -> next) {
            printf("**COALESCE RIGHT**\n");
            if ((((unsigned char*)toFree) + sizeof(struct malloc_stc) + (toFree -> size) == ((unsigned char*)toFree -> next)) || ((((unsigned char*)toFree->next) - (toFree -> size)  + sizeof(struct malloc_stc)) == ((unsigned char*)toFree))) {
                toFree -> next -> size += toFree -> size + sizeof(struct malloc_stc);
                toFree -> size = toFree -> next -> size;
                struct malloc_stc* tempNext = toFree -> next;
                toFree -> next = tempNext -> next;
                if (hasCoalescedPrev) {
                    size_ptr -> size = tempNext -> size; //CHECK THIS LINE
                }
                if(size_ptr) {
                    if(size_ptr -> size == tempNext -> size) {
                        if(size_ptr -> size == MyFreeList -> size) {
                            size_ptr -> next = NULL;
                        }
                    }
                }
                tempNext -> prev = NULL; //sets tempNext prev pointer to NULL when should be pointing to toFree
                tempNext -> next = NULL;
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
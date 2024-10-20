#define _GNU_SOURCE // for MAP_ANONYMOUS or other macros if needed.
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "mymalloc.h"

// force headers to align to 16 bytes.
#define ALIGN 16
union block
{
       struct data {
        union block *next;
        size_t size;
        int is_free;
    } data;
    char stub[ALIGN];
};

block_t *head = NULL, *tail = NULL;

pthread_mutex_t global_lock;

block_t *get_free_block(size_t size)
{
    block_t *curr = head;
    while (curr)
    {
        if (curr->data.is_free && curr->data.size >= size)
        {
            return curr;
        }
        curr = curr->data.next;
    }
    return NULL;
}

void myfree(void *block)
{
    if (!block)
        return;

    pthread_mutex_lock(&global_lock);

    block_t *header, *tmp;
    void *prog_break;

    header = (block_t *) block - 1;

    // sbrk(0) returns current program break.
    prog_break = sbrk(0);

    // if this is the last block, i.e. tail
    if ((char *) block + header->data.size == prog_break) {
        if (head == tail) {
            // empty linked list
            head = tail = NULL;
        } else {
            // update second to last block to be tail
            tmp = head;

            while (tmp != NULL) {
                if (tmp->data.next == tail) {
                    tmp->data.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->data.next;
            }

            // deallocate the memory
            sbrk(0 - header->data.size - sizeof(block_t));
            pthread_mutex_unlock(&global_lock);
            return;
        }
    }
    // otherwise, simply mark block as free.
    header->data.is_free = 1;
    pthread_mutex_unlock(&global_lock);
    return;
}

void *mymalloc(size_t size)
{
    if (!size)
        return NULL;

    pthread_mutex_lock(&global_lock);

    block_t *header = get_free_block(size);

    // if free block found.
    if (header)
    {
        header->data.is_free = 0;
        pthread_mutex_unlock(&global_lock);
        return (void *)(++header); // point to allocated memory (hide header).
    }

    // allocate new heap memory.
    size_t total_size = sizeof(block_t) + size; // header plus block, need to allocate memory for both.

    block_t *new_block;
    new_block = sbrk(total_size);
    if (new_block == (void *)-1) {
        // sbrk error
        pthread_mutex_unlock(&global_lock);
		return NULL;
    }

    header = new_block;

    header->data.size = size;
    header->data.is_free = 0;
    header->data.next = NULL;

    // very fist block.
    if (!head)
    {
        head = header;
    }

    // if tail, point tail (aka prev last block)'s next to new tail
    if (tail)
    {
        tail->data.next = header;
    }

    // update new tail.
    tail = header;
    pthread_mutex_unlock(&global_lock);
    
    // return pointer to block, not header.
    return (void *)(++header);
}

void *mycalloc(size_t nitems, size_t size)
{
    if (!nitems || !size)
        return NULL;
    size_t total_size = nitems * size;
    void *block = mymalloc(total_size);
    if (!block)
        return NULL;
    memset(block, 0, total_size);
    return block;
}

void *myrealloc(void *block, size_t size)
{
    if (!block || !size)
        return NULL;

    block_t *header = (block_t *)block - 1;

    if (header->data.size >= size)
    {
        return block;
    }

    void *new_block = mymalloc(size);

    if (new_block)
    {
        memcpy(new_block, block, size);
        myfree(block);
        return new_block;
    }
    return NULL; // if mymalloc failed.
}

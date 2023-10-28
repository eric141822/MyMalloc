#define _GNU_SOURCE // for MAP_ANONYMOUS or other macros if needed.
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "mymalloc.h"

struct block
{
    struct block *next;
    size_t size;
    int is_free;
};

block_t *head = NULL, *tail = NULL;

pthread_mutex_t global_lock;

block_t *get_free_block(size_t size)
{
    block_t *curr = head;
    while (curr)
    {
        if (curr->is_free && curr->size >= size)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void myfree(void *block)
{
    if (!block)
        return;

    pthread_mutex_lock(&global_lock);
    block_t *header = (block_t *)block - 1;
    if (tail == header)
    {
        size_t free_size = header->size - sizeof(block_t);
        munmap(header, free_size);
        pthread_mutex_unlock(&global_lock);
        return;
    }
    header->is_free = 1;
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
        header->is_free = 0;
        pthread_mutex_unlock(&global_lock);
        return (void *)(++header); // point to allocated memory (hide header).
    }

    size_t total_size = sizeof(block_t) + size; // header plus block, need to allocate memory for both.

    block_t *new_block;
    if ((new_block = mmap(NULL, total_size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == MAP_FAILED)
    {
        fprintf(stderr, "mmap failed\n");
        pthread_mutex_unlock(&global_lock);
        return NULL;
    }

    new_block->size = size;
    new_block->is_free = 0;
    new_block->next = NULL;

    if (!head)
    {
        head = new_block;
    }
    if (tail)
    {
        tail->next = new_block;
    }
    tail = new_block;
    pthread_mutex_unlock(&global_lock);
    // return pointer to block, not header.
    return (void *)(++new_block);
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

    if (header->size >= size)
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

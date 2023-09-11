#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include "mymalloc.h"

typedef char STUB[16]; // intel specified, aligned to 16 bytes.

union block
{
    STUB stub;
    struct
    {
        union block *next;
        size_t size;
        int is_free;
    } s;
};

block_t *head = NULL, *tail = NULL;

pthread_mutex_t global_lock;

block_t *get_free_block(size_t size)
{
    block_t *curr = head;
    while (curr)
    {
        if (curr->s.is_free && curr->s.size >= size)
        {
            return curr;
        }
        curr = curr->s.next;
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
        size_t free_size = 0 - header->s.size - sizeof(block_t);
        munmap(header, free_size);
        pthread_mutex_unlock(&global_lock);
        return;
    }
    header->s.is_free = 1;
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
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_lock);
        return (void *)(header + 1); // point to the block (not header).
    }

    size_t total_size = sizeof(block_t) + size; // header plus block, need to allocate memory for both.

    block_t *new_block;

    if ((new_block = mmap(NULL, total_size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == MAP_FAILED)
    {
        fprintf(stderr, "mmap failed\n");
        pthread_mutex_unlock(&global_lock);
        return NULL;
    }

    new_block->s.size = size;
    new_block->s.is_free = 0;
    new_block->s.next = NULL;

    if (!head)
    {
        head = new_block;
    }
    if (tail)
    {
        tail->s.next = new_block;
    }
    tail = new_block;
    pthread_mutex_unlock(&global_lock);
    // return pointer to block, not header.
    return (void *)(new_block + 1);
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

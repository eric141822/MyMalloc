#ifndef MYMALLOC_H
#define MYMALLOC_H

typedef struct block block_t;

block_t *get_free_block(size_t size);

void myfree(void *block);

void *mymalloc(size_t size);

void *mycalloc(size_t nitems, size_t size);

void *myrealloc(void *block, size_t size);

#endif /* MYMALLOC.H */


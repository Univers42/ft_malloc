#include "alloc.h"
#include <sys/mman.h>

/* Allocate memory for a zone using the configured method */
static void *zone_allocate_memory(size_t size)
{
#if USE_SBRK_FOR_TINY_SMALL
    /* Hybrid mode: use sbrk for zones */
    void *ptr = sbrk(size);
    if (ptr == (void *)-1)
        return NULL;
    return ptr;
#else
    /* Pure mmap mode (project requirement) */
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED)
        return NULL;
    return ptr;
#endif
}

/* Create a new zone for given block size */
t_zone *create_zone(size_t block_size, size_t zone_size)
{
    t_zone *zone;
    void *mem;
    size_t usable_size;
    size_t num_blocks;
    t_mhead *block;
    size_t i;

    /* Allocate zone memory */
    mem = zone_allocate_memory(zone_size);
    if (!mem)
        return NULL;

    /* Zone header at the start */
    zone = (t_zone *)mem;
    zone->start = (char *)mem + sizeof(t_zone);
    zone->end = (char *)mem + zone_size;
    zone->block_size = block_size;
    zone->next = NULL;

    /* Calculate blocks */
    usable_size = zone_size - sizeof(t_zone);
    num_blocks = usable_size / (block_size + sizeof(t_mhead));
    zone->total_blocks = num_blocks;
    zone->used_blocks = 0;

    /* Build free list */
    zone->free_list = (t_mhead *)zone->start;
    block = zone->free_list;
    i = 0;
    while (i < num_blocks - 1)
    {
        block->s_minfo.mi_alloc = ISFREE;
        *chain_ptr(block) = (t_mhead *)((char *)block + block_size + sizeof(t_mhead));
        block = *chain_ptr(block);
        i++;
    }
    block->s_minfo.mi_alloc = ISFREE;
    *chain_ptr(block) = NULL;

    return zone;
}

/* Find or create a zone with a free block */
t_zone *get_zone_with_space(t_zone **zone_list, size_t block_size, size_t zone_size)
{
    t_zone *zone;

    /* Search existing zones */
    zone = *zone_list;
    while (zone)
    {
        if (zone->free_list != NULL)
            return zone;
        zone = zone->next;
    }

    /* Create new zone */
    zone = create_zone(block_size, zone_size);
    if (!zone)
        return NULL;

    /* Add to list */
    zone->next = *zone_list;
    *zone_list = zone;
    return zone;
}

/* Allocate a block from a zone */
void *allocate_from_zone(t_zone *zone)
{
    t_mhead *block;
    void *ptr;

    if (!zone || !zone->free_list)
        return NULL;

    block = zone->free_list;
    zone->free_list = *chain_ptr(block);
    zone->used_blocks++;

    block->s_minfo.mi_alloc = ISALLOC;
    ptr = (void *)(block + 1);
    return ptr;
}

/* Free a block back to its zone */
void free_to_zone(t_zone *zone, t_mhead *block)
{
    if (!zone || !block)
        return;

    block->s_minfo.mi_alloc = ISFREE;
    *chain_ptr(block) = zone->free_list;
    zone->free_list = block;
    zone->used_blocks--;
}

#ifndef DEBUG_H
#define DEBUG_H

#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>

#define TINY_MAX 128
#define SMALL_MAX 1024
#define MAX_TRACKED_ALLOCS 10000

typedef struct s_alloc_entry
{
    void *ptr;
    size_t size;
    int active;
} t_alloc_entry;

#endif
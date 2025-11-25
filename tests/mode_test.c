#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alloc.h"

/*
 * If MODE_MALLOC is defined we use the system allocator (malloc/realloc/free).
 * Otherwise we use the custom allocator (ft_malloc/ft_realloc/ft_free).
 */
#ifdef MODE_MALLOC
#define ALLOC_FUNC malloc
#define REALLOC_FUNC realloc
#define FREE_FUNC free
#define ALLOC_NAME "libc malloc"
#else
#define ALLOC_FUNC ft_malloc
#define REALLOC_FUNC ft_realloc
#define FREE_FUNC ft_free
#define ALLOC_NAME "ft_malloc"
#endif

static void run_test(void)
{
    void *p;
    void *q;

    printf("=== Running test with %s ===\n", ALLOC_NAME);

    /* allocate */
    p = ALLOC_FUNC(64);
    if (!p)
    {
        fprintf(stderr, "alloc(64) failed\n");
        return;
    }
    strcpy((char *)p, "allocator test string");
    printf("alloc(64) -> %p : \"%s\"\n", p, (char *)p);

    /* realloc larger */
    q = REALLOC_FUNC(p, 256);
    if (!q)
    {
        fprintf(stderr, "realloc -> 256 failed\n");
        /* if realloc returned NULL and didn't free p (libc semantics),
         * free original p when possible.
         */
#ifdef MODE_MALLOC
        FREE_FUNC(p);
#else
        /* ft_realloc semantics assumed to free or return NULL; try to free p */
        FREE_FUNC(p);
#endif
        return;
    }
    printf("realloc(256) -> %p\n", q);
    strcat((char *)q, " -- extended");
    printf("after realloc content: \"%s\"\n", (char *)q);

    /* shrink with realloc */
    p = REALLOC_FUNC(q, 32);
    if (!p)
    {
        fprintf(stderr, "realloc -> 32 failed\n");
        return;
    }
    printf("realloc(32) -> %p\n", p);

    /* optional: inspect allocator internal state (only meaningful for ft_malloc) */
#ifndef MODE_MALLOC
    show_alloc_mem();
#endif

    /* free */
    FREE_FUNC(p);
    printf("free done\n");

#ifndef MODE_MALLOC
    show_alloc_mem();
#endif
}

int main(void)
{
    /* Run small sequence a couple of times */
    run_test();
    printf("\n-- run again --\n\n");
    run_test();
    return 0;
}

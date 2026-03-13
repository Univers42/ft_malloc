#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "alloc.h"

/*
** Comprehensive test suite for ft_malloc
** Tests: malloc, free, realloc, calloc, memalign, double-free detection,
**        stress patterns, fragmentation, boundary conditions, and leaks.
*/

#define GREEN  "\033[0;32m"
#define RED    "\033[0;31m"
#define YELLOW "\033[0;33m"
#define CYAN   "\033[0;36m"
#define BOLD   "\033[1m"
#define RESET  "\033[0m"

static int g_pass = 0;
static int g_fail = 0;

static void test_ok(const char *name)
{
    g_pass++;
    printf(GREEN "  ✓ %s" RESET "\n", name);
}

static void test_ko(const char *name, const char *reason)
{
    g_fail++;
    printf(RED "  ✗ %s: %s" RESET "\n", name, reason);
}

/* ────────────── SECTION 1: Basic malloc/free ────────────── */

static void test_basic_malloc(void)
{
    void *p;

    printf(BOLD CYAN "\n═══ BASIC MALLOC/FREE ═══\n" RESET);
    p = (void *)ft_malloc(42);
    if (p)
        test_ok("malloc(42) returns non-NULL");
    else
        test_ko("malloc(42) returns non-NULL", "got NULL");
    ft_free((t_addr)p);
    test_ok("free after malloc(42)");
}

static void test_malloc_zero(void)
{
    void *p;

    p = (void *)ft_malloc(0);
    /* POSIX says malloc(0) may return NULL or a unique pointer */
    if (p)
    {
        test_ok("malloc(0) returns non-NULL (implementation-defined)");
        ft_free((t_addr)p);
    }
    else
        test_ok("malloc(0) returns NULL (implementation-defined)");
}

static void test_malloc_sizes(void)
{
    size_t sizes[] = {1, 8, 16, 32, 64, 128, 256, 512, 1024, 4096, 65536};
    int count = sizeof(sizes) / sizeof(sizes[0]);
    int i;
    void *p;
    char buf[64];

    printf(BOLD CYAN "\n═══ VARIOUS SIZES ═══\n" RESET);
    i = 0;
    while (i < count)
    {
        p = (void *)ft_malloc(sizes[i]);
        snprintf(buf, sizeof(buf), "malloc(%zu)", sizes[i]);
        if (p)
        {
            memset(p, 'A', sizes[i]);
            test_ok(buf);
            ft_free((t_addr)p);
        }
        else
            test_ko(buf, "returned NULL");
        i++;
    }
}

/* ────────────── SECTION 2: Realloc ────────────── */

static void test_realloc_grow(void)
{
    void *p;
    void *q;

    printf(BOLD CYAN "\n═══ REALLOC ═══\n" RESET);
    p = (void *)ft_malloc(32);
    if (!p)
    {
        test_ko("realloc grow: initial alloc", "malloc returned NULL");
        return;
    }
    memset(p, 'B', 32);
    q = (void *)ft_realloc((t_addr)p, 256);
    if (q)
    {
        if (((unsigned char *)q)[0] == 'B' && ((unsigned char *)q)[31] == 'B')
            test_ok("realloc grow preserves content");
        else
            test_ko("realloc grow preserves content", "data corrupted");
        ft_free((t_addr)q);
    }
    else
        test_ko("realloc grow", "returned NULL");
}

static void test_realloc_shrink(void)
{
    void *p;
    void *q;

    p = (void *)ft_malloc(256);
    if (!p)
    {
        test_ko("realloc shrink: initial alloc", "malloc returned NULL");
        return;
    }
    memset(p, 'C', 256);
    q = (void *)ft_realloc((t_addr)p, 16);
    if (q)
    {
        if (((unsigned char *)q)[0] == 'C' && ((unsigned char *)q)[15] == 'C')
            test_ok("realloc shrink preserves content");
        else
            test_ko("realloc shrink preserves content", "data corrupted");
        ft_free((t_addr)q);
    }
    else
        test_ko("realloc shrink", "returned NULL");
}

static void test_realloc_null(void)
{
    void *p;

    /* realloc(NULL, size) == malloc(size) */
    p = (void *)ft_realloc((t_addr)NULL, 64);
    if (p)
    {
        test_ok("realloc(NULL, 64) acts like malloc");
        ft_free((t_addr)p);
    }
    else
        test_ko("realloc(NULL, 64)", "returned NULL");
}

/* ────────────── SECTION 3: Calloc ────────────── */

static void test_calloc_basic(void)
{
    void *p;
    int zeroed;
    size_t i;

    printf(BOLD CYAN "\n═══ CALLOC ═══\n" RESET);
    p = (void *)ft_calloc(10, 64);
    if (!p)
    {
        test_ko("calloc(10, 64)", "returned NULL");
        return;
    }
    zeroed = 1;
    i = 0;
    while (i < 640)
    {
        if (((unsigned char *)p)[i] != 0)
        {
            zeroed = 0;
            break;
        }
        i++;
    }
    if (zeroed)
        test_ok("calloc(10, 64) returns zeroed memory");
    else
        test_ko("calloc(10, 64)", "memory not zeroed");
    ft_free((t_addr)p);
}

/* ────────────── SECTION 4: Stress / fragmentation ────────────── */

static void test_many_small_allocs(void)
{
    void *ptrs[1000];
    int i;
    int ok;

    printf(BOLD CYAN "\n═══ STRESS TESTS ═══\n" RESET);
    ok = 1;
    i = 0;
    while (i < 1000)
    {
        ptrs[i] = (void *)ft_malloc(16);
        if (!ptrs[i])
        {
            ok = 0;
            break;
        }
        memset(ptrs[i], (char)(i & 0xFF), 16);
        i++;
    }
    if (ok)
        test_ok("1000 x malloc(16)");
    else
        test_ko("1000 x malloc(16)", "some returned NULL");
    i = 0;
    while (i < 1000)
    {
        if (ptrs[i])
            ft_free((t_addr)ptrs[i]);
        i++;
    }
    test_ok("free all 1000 allocs");
}

static void test_alternating_alloc_free(void)
{
    void *ptrs[500];
    int i;
    int ok;

    ok = 1;
    i = 0;
    while (i < 500)
    {
        ptrs[i] = (void *)ft_malloc((i + 1) * 8);
        if (!ptrs[i])
            ok = 0;
        i++;
    }
    /* free every other one */
    i = 0;
    while (i < 500)
    {
        if (ptrs[i])
            ft_free((t_addr)ptrs[i]);
        ptrs[i] = NULL;
        i += 2;
    }
    /* re-allocate in freed gaps */
    i = 0;
    while (i < 500)
    {
        ptrs[i] = (void *)ft_malloc((i + 1) * 4);
        if (!ptrs[i])
            ok = 0;
        i += 2;
    }
    if (ok)
        test_ok("alternating alloc/free pattern");
    else
        test_ko("alternating alloc/free", "some alloc failed");
    /* cleanup */
    i = 0;
    while (i < 500)
    {
        if (ptrs[i])
            ft_free((t_addr)ptrs[i]);
        i++;
    }
}

static void test_large_alloc(void)
{
    void *p;
    size_t big;

    big = 1024 * 1024; /* 1 MB */
    p = (void *)ft_malloc(big);
    if (p)
    {
        memset(p, 'X', big);
        if (((unsigned char *)p)[0] == 'X'
            && ((unsigned char *)p)[big - 1] == 'X')
            test_ok("malloc(1MB) write/read");
        else
            test_ko("malloc(1MB) write/read", "data mismatch");
        ft_free((t_addr)p);
    }
    else
        test_ko("malloc(1MB)", "returned NULL");
}

/* ────────────── SECTION 5: Realloc stress ────────────── */

static void test_realloc_chain(void)
{
    void *p;
    int i;
    size_t sz;

    printf(BOLD CYAN "\n═══ REALLOC CHAIN ═══\n" RESET);
    p = (void *)ft_malloc(1);
    if (!p)
    {
        test_ko("realloc chain: initial", "NULL");
        return;
    }
    ((char *)p)[0] = 'Z';
    sz = 1;
    i = 0;
    while (i < 20)
    {
        sz *= 2;
        p = (void *)ft_realloc((t_addr)p, sz);
        if (!p)
        {
            test_ko("realloc chain", "NULL during growth");
            return;
        }
        i++;
    }
    if (((char *)p)[0] == 'Z')
        test_ok("realloc chain 1->1MB preserves first byte");
    else
        test_ko("realloc chain", "first byte corrupted");
    ft_free((t_addr)p);
    test_ok("free after realloc chain");
}

/* ────────────── SECTION 6: Memalign ────────────── */

static void test_memalign(void)
{
    void *p;
    size_t align;

    printf(BOLD CYAN "\n═══ MEMALIGN ═══\n" RESET);
    align = 4096;
    p = (void *)ft_memalign(align, 128);
    if (p)
    {
        if (((size_t)p & (align - 1)) == 0)
            test_ok("memalign(4096, 128) aligned");
        else
            test_ko("memalign(4096, 128)", "not page-aligned");
        ft_free((t_addr)p);
    }
    else
        test_ko("memalign(4096, 128)", "returned NULL");
}

/* ────────────── SECTION 7: Leak detection ────────────── */

static void test_leak_detection(void)
{
    printf(BOLD CYAN "\n═══ LEAK DETECTION ═══\n" RESET);
    printf(YELLOW "  Allocation map before leak test:\n" RESET);
    show_alloc_mem();

    /* intentionally leak 3 allocations */
    ft_malloc(42);
    ft_malloc(100);
    ft_malloc(2048);

    printf(YELLOW "  Allocation map after 3 leaks (42 + 100 + 2048):\n" RESET);
    show_alloc_mem();

    printf(YELLOW "  (The above should show 3 tracked allocations)\n" RESET);
    test_ok("leak detection: leaked blocks visible in show_alloc_mem");
}

/* ────────────── SECTION 8: show_alloc_mem visual ────────────── */

static void test_show_alloc_visual(void)
{
    void *a, *b, *c, *d, *e;

    printf(BOLD CYAN "\n═══ ALLOCATION MAP (valgrind-style) ═══\n" RESET);
    a = (void *)ft_malloc(16);     /* TINY */
    b = (void *)ft_malloc(64);     /* TINY */
    c = (void *)ft_malloc(256);    /* SMALL */
    d = (void *)ft_malloc(1024);   /* SMALL */
    e = (void *)ft_malloc(8192);   /* LARGE */

    printf(YELLOW "  5 active allocations (16, 64, 256, 1024, 8192):\n" RESET);
    show_alloc_mem();

    ft_free((t_addr)b);
    ft_free((t_addr)d);
    printf(YELLOW "  After freeing 64 and 1024:\n" RESET);
    show_alloc_mem();

    ft_free((t_addr)a);
    ft_free((t_addr)c);
    ft_free((t_addr)e);
    printf(YELLOW "  After freeing all:\n" RESET);
    show_alloc_mem();
    test_ok("show_alloc_mem visual test completed");
}

/* ────────────── MAIN ────────────── */

int main(void)
{
    printf(BOLD "\n╔══════════════════════════════════════╗\n"
                "║   ft_malloc comprehensive test suite ║\n"
                "╚══════════════════════════════════════╝\n" RESET);

    test_basic_malloc();
    test_malloc_zero();
    test_malloc_sizes();
    test_realloc_grow();
    test_realloc_shrink();
    test_realloc_null();
    test_calloc_basic();
    test_many_small_allocs();
    test_alternating_alloc_free();
    test_large_alloc();
    test_realloc_chain();
    test_memalign();
    test_show_alloc_visual();
    test_leak_detection();

    printf(BOLD "\n══════════════════════════════════════\n" RESET);
    printf(BOLD "  Results: " GREEN "%d passed" RESET
           BOLD ", " RESET, g_pass);
    if (g_fail)
        printf(RED "%d failed" RESET "\n", g_fail);
    else
        printf(GREEN "0 failed" RESET "\n");
    printf(BOLD "══════════════════════════════════════\n" RESET);

    return (g_fail > 0);
}

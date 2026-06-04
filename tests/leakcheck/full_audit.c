/*
 * full_audit.c -- ft_malloc leak & correctness harness (the "inner tester").
 *
 * Lanes that drive this binary:
 *   - Lane 1 (syscall interposer): run under LD_PRELOAD=bin/libsyscount.so; the
 *     weak syscount_* hooks below turn the OS-leak checks into hard assertions.
 *   - Lane 2 (call-site leak map): FT_MALLOC_DEBUG routes ft_malloc/... through
 *     the *_dbg wrappers so malloc_leak_map() can report file:line of live blocks.
 *   - Lane 3/4 (ASan / Valgrind): same source, built/run without the interposer.
 *
 * After every section we assert the internal tracker is back to zero
 * (malloc_live_count()==0 && malloc_live_bytes()==0). The death tests run in a
 * forked child and assert the allocator aborts (SIGABRT via botch/xbotch).
 *
 * Run `full_audit` for the PASS/FAIL audit; `full_audit --leak-demo` to also show
 * the call-site leak map catching a deliberate leak.
 */

#include "alloc.h"
#ifndef FT_MALLOC_DEBUG
# define FT_MALLOC_DEBUG
#endif
#include "ft_malloc_debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

/* weak hooks provided by the Lane-1 interposer; NULL when not preloaded */
__attribute__((weak)) void			syscount_baseline(void);
__attribute__((weak)) void			syscount_report(void);
__attribute__((weak)) unsigned long	syscount_live_since_baseline(void);

static int	g_pass;
static int	g_fail;

static void	check(const char *label, int ok)
{
	if (ok)
	{
		g_pass++;
		printf("  \033[0;32mPASS\033[0m %s\n", label);
	}
	else
	{
		g_fail++;
		printf("  \033[0;31mFAIL\033[0m %s\n", label);
	}
}

static void	expect_clean(const char *section)
{
	char	buf[160];

	snprintf(buf, sizeof(buf), "%s: tracker back to zero (count=%zu bytes=%zu)",
		section, malloc_live_count(), malloc_live_bytes());
	check(buf, malloc_live_count() == 0 && malloc_live_bytes() == 0);
}

static void	section(const char *name)
{
	printf("\n\033[1m── %s ──\033[0m\n", name);
}

/* ------------------------------------------------------------------ sizes */
static const size_t	g_sizes[] = {
	0, 1, 8, 16, 17, 32, 64, 128, 129, 256, 512, 1024, 1025, 4096, 4097,
	16384, 1048576
};
#define NSIZES (sizeof(g_sizes) / sizeof(g_sizes[0]))

static void	test_api_size_matrix(void)
{
	size_t	i;
	char	*p;

	section("API x size matrix (malloc/free, all classes)");
	i = (size_t)-1;
	while (++i < NSIZES)
	{
		p = ft_malloc(g_sizes[i]);
		if (g_sizes[i] > 0)
		{
			if (!p)
			{
				check("malloc returned non-NULL", 0);
				continue ;
			}
			memset(p, 0xab, g_sizes[i]);
			p[g_sizes[i] - 1] = 0x7f;
		}
		ft_free(p);
	}
	check("all sizes alloc+write+free without crash", 1);
	expect_clean("api-matrix");
}

static void	test_calloc(void)
{
	int		*a;
	int		i;
	int		zeroed;

	section("calloc (zeroing + n*s)");
	a = ft_calloc(256, sizeof(int));
	check("calloc non-NULL", a != NULL);
	zeroed = 1;
	i = -1;
	while (a && ++i < 256)
		if (a[i] != 0)
			zeroed = 0;
	check("calloc returns zeroed memory", zeroed);
	ft_free(a);
	a = ft_calloc(0, 0);
	ft_free(a);
	expect_clean("calloc");
}

static void	test_realloc_semantics(void)
{
	char	*p;
	char	*q;
	int		i;
	int		preserved;

	section("realloc semantics + data integrity");
	p = ft_malloc(100);
	i = -1;
	while (++i < 100)
		p[i] = (char)(i & 0x7f);
	q = ft_realloc(p, 400);
	preserved = 1;
	i = -1;
	while (++i < 100)
		if (q[i] != (char)(i & 0x7f))
			preserved = 0;
	check("realloc grow preserves bytes", preserved);
	q = ft_realloc(q, 40);
	preserved = 1;
	i = -1;
	while (++i < 40)
		if (q[i] != (char)(i & 0x7f))
			preserved = 0;
	check("realloc shrink preserves bytes", preserved);
	ft_free(q);
	q = ft_realloc(NULL, 64);
	check("realloc(NULL, n) behaves like malloc", q != NULL);
	ft_free(q);
	q = ft_realloc(ft_malloc(64), 0);
	check("realloc(p, 0) frees and returns NULL", q == NULL);
	expect_clean("realloc");
}

static void	test_bug1_realloc_inplace(void)
{
	void	*a;
	void	*b;
	size_t	base;

	section("BUG#1 regression: in-place realloc updates tracked bytes");
	a = ft_malloc(8);
	base = malloc_live_bytes();
	b = ft_realloc(a, 12);
	printf("  (in-place=%d) live_bytes now %zu, expected %zu\n",
		b == a, malloc_live_bytes(), base - 8 + 12);
	check("tracked bytes follow in-place realloc (8->12)",
		malloc_live_bytes() == base - 8 + 12);
	ft_free(b);
	expect_clean("realloc-inplace");
}

static void	test_offset_alignment(void)
{
	char	*p;
	t_mhead	*h;

	section("offset / alignment (header + guard geometry)");
	p = ft_malloc(64);
	check("returned pointer is 16-byte aligned", ((uintptr_t)p & 15) == 0);
	h = (t_mhead *)p - 1;
	check("header sits exactly sizeof(t_mhead) before payload",
		(char *)p - (char *)h == (long)sizeof(t_mhead));
	check("header records ISALLOC status", h->s_minfo.mi_alloc == ISALLOC);
	check("header records requested size", h->s_minfo.mi_nbytes == 64);
	check("header magic2 intact", h->s_minfo.mi_magic2 == MAGIC2);
	memset(p, 0x11, 64);
	ft_free(p);
	expect_clean("offset");
}

static void	test_memalign_valloc(void)
{
	void	*m;
	void	*v;

	section("BUG#3 regression: memalign/valloc alignment + tracker");
	m = ft_memalign(4096, 100);
	check("memalign(4096) returns 4096-aligned", ((uintptr_t)m & 4095) == 0);
	ft_free(m);
	check("memalign block fully untracked after free (count==0)",
		malloc_live_count() == 0);
	v = ft_valloc(200);
	check("valloc returns page-aligned",
		((uintptr_t)v & (uintptr_t)(getpagesize() - 1)) == 0);
	ft_free(v);
	expect_clean("memalign-valloc");
}

static void	test_bouncing(void)
{
	void	*live[256];
	int		i;
	int		op;
	size_t	mine;

	section("bouncing: randomized alloc/free churn (drift detection)");
	memset(live, 0, sizeof(live));
	mine = 0;
	srand(1234567);
	i = -1;
	while (++i < 40000)
	{
		op = rand() % 256;
		if (live[op] == NULL)
		{
			live[op] = ft_malloc((size_t)(rand() % 2000) + 1);
			if (live[op])
				mine++;
		}
		else
		{
			ft_free(live[op]);
			live[op] = NULL;
			mine--;
		}
		if ((i & 8191) == 0 && malloc_live_count() != mine)
			break ;
	}
	check("internal live-count tracks harness count through churn",
		malloc_live_count() == mine);
	i = -1;
	while (++i < 256)
		if (live[i])
			ft_free(live[i]);
	expect_clean("bouncing");
}

/*
 * Large blocks are now cached on free (LARGE_CACHE_CAP) instead of munmap-ed
 * per free. Verify two contracts: (1) a freed large block is cached and reused
 * by the next same-size alloc (no new mmap); (2) holding more large blocks live
 * than the cap and freeing them all returns the excess to the OS (bounded
 * retention — no unbounded OS leak).
 */
#define NBIG 80
#define BIGSZ (2 * 1024 * 1024)

static void	test_os_leak_large(void)
{
	void			*big[NBIG];
	unsigned long	base;
	unsigned long	peak;
	int				i;

	section("OS: large-block cache reuse + bounded retention (Lane 1)");
	if (!syscount_live_since_baseline)
	{
		printf("  (interposer not preloaded; skipping OS-level checks)\n");
		return ;
	}
	base = syscount_live_since_baseline();
	big[0] = ft_malloc(BIGSZ);
	check("large alloc creates one mmap region",
		syscount_live_since_baseline() == base + 1);
	ft_free(big[0]);
	big[0] = ft_malloc(BIGSZ);
	check("freed large block is cached and reused (no new mmap)",
		syscount_live_since_baseline() == base + 1);
	ft_free(big[0]);
	i = -1;
	while (++i < NBIG)
		big[i] = ft_malloc(BIGSZ);
	peak = syscount_live_since_baseline();
	i = -1;
	while (++i < NBIG)
		ft_free(big[i]);
	check("holding > cap large blocks then freeing returns the excess to OS",
		syscount_live_since_baseline() < peak
		&& syscount_live_since_baseline() <= base + (LARGE_CACHE_CAP / BIGSZ) + 2);
	expect_clean("os-leak-large");
}

/* ------------------------------------------------------------- death tests */
static void	death_double_free(void)
{
	void	*p;

	p = ft_malloc(32);
	ft_free(p);
	ft_free(p);
}

static void	death_magic8_underflow(void)
{
	char	*p;

	p = ft_malloc(64);
	p[-1] = 0;
	ft_free(p);
}

static void	death_endguard_overflow(void)
{
	char	*p;

	p = ft_malloc(64);
	p[64] = 0x42;
	ft_free(p);
}

static void	death_interior_free(void)
{
	char	*p;

	p = ft_malloc(64);
	ft_free(p + 8);
}

static void	run_death(const char *label, void (*fn)(void))
{
	pid_t	pid;
	int		st;
	int		fd;

	pid = fork();
	if (pid == 0)
	{
		fd = open("/dev/null", O_WRONLY);
		if (fd >= 0)
		{
			dup2(fd, 1);
			dup2(fd, 2);
		}
		fn();
		_exit(0);
	}
	waitpid(pid, &st, 0);
	check(label, WIFSIGNALED(st) && WTERMSIG(st) == SIGABRT);
}

static void	test_debounce_corruption(void)
{
	section("debounce: double-free / corruption must abort (subprocess)");
	run_death("double free aborts", death_double_free);
	run_death("header underflow (magic8) aborts", death_magic8_underflow);
	run_death("buffer overflow (end guard) aborts", death_endguard_overflow);
	run_death("free of interior pointer aborts", death_interior_free);
	ft_free(NULL);
	check("free(NULL) is a safe no-op", 1);
	expect_clean("debounce");
}

static void	warmup_and_baseline(void)
{
	void	*a;
	void	*b;

	a = ft_malloc(64);
	b = ft_malloc(2048);
	ft_free(a);
	ft_free(b);
	if (syscount_baseline)
		syscount_baseline();
}

static int	leak_demo(void)
{
	printf("\n\033[1m── DEMO: intentional leak (call-site map) ──\033[0m\n");
	ft_malloc(42);
	ft_malloc(300);
	ft_malloc(5000);
	malloc_leak_map();
	if (syscount_report)
		syscount_report();
	return (0);
}

int	main(int argc, char **argv)
{
	printf("\033[1m=== ft_malloc full audit ===\033[0m\n");
	warmup_and_baseline();
	test_api_size_matrix();
	test_calloc();
	test_realloc_semantics();
	test_bug1_realloc_inplace();
	test_offset_alignment();
	test_memalign_valloc();
	test_bouncing();
	test_os_leak_large();
	test_debounce_corruption();
	printf("\n\033[1m=== final tracker state ===\033[0m\n");
	malloc_leak_map();
	if (syscount_report)
		syscount_report();
	printf("\n\033[1mRESULTS: %d passed, %d failed\033[0m\n", g_pass, g_fail);
	printf("LEAKCHECK_RESULT: %s\n", g_fail == 0 ? "PASS" : "FAIL");
	if (argc > 1 && !strcmp(argv[1], "--leak-demo"))
		leak_demo();
	return (g_fail == 0 ? 0 : 1);
}

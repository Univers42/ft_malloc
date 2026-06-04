/*
 * bench.c -- ft_malloc vs libc micro-benchmark (~50 scenarios).
 *
 * ft_malloc does NOT interpose libc, so one process can call both ft_* and the
 * system *alloc. Each scenario runs the identical workload through each allocator
 * (warm-up + best-of-REPS wall time) and reports ns/op and who won.
 *
 * Build: the allocator sources are compiled straight in. `make bench` links a
 * no-op tracker (pure-allocator comparison); `make bench-shipped` keeps the real
 * tracker to show its cost.
 */
#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <malloc.h>

typedef void	*(*t_mfn)(size_t);
typedef void	(*t_ffn)(void *);
typedef void	*(*t_rfn)(void *, size_t);
typedef void	*(*t_cfn)(size_t, size_t);
typedef void	*(*t_afn)(size_t, size_t);

typedef struct s_alloc
{
	const char	*name;
	t_mfn		mal;
	t_ffn		fre;
	t_rfn		rea;
	t_cfn		cal;
	t_afn		ali;
}	t_alloc;

enum
{
	K_CHURN, K_BATCH_LIFO, K_BATCH_FIFO, K_CALLOC, K_REALLOC, K_MEMALIGN, K_MIXED
};

typedef struct s_scn
{
	const char	*name;
	int			kind;
	size_t		a;
	size_t		b;
	long		n;
	long		iters;
}	t_scn;

#define REPS 7
#define MAXLIVE 8192

static volatile unsigned long	g_sink;
static void						*g_buf[MAXLIVE];

static void	*libc_memalign(size_t align, size_t size)
{
	/* glibc memalign(), NOT posix_memalign() -- the allocator under test
	 * defines a posix_memalign symbol that would shadow libc's here. */
	return (memalign(align, size));
}

static double	now_ns(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((double)ts.tv_sec * 1e9 + (double)ts.tv_nsec);
}

static unsigned int	xrand(unsigned int *s)
{
	*s ^= *s << 13;
	*s ^= *s >> 17;
	*s ^= *s << 5;
	return (*s);
}

static long	do_churn(const t_alloc *al, const t_scn *s)
{
	long	i;
	char	*p;

	for (i = 0; i < s->iters; i++)
	{
		p = al->mal(s->a);
		if (p)
		{
			p[0] = (char)i;
			p[s->a - 1] = (char)i;
			g_sink += (unsigned long)(unsigned char)p[0];
			al->fre(p);
		}
	}
	return (s->iters);
}

static long	do_batch(const t_alloc *al, const t_scn *s, int lifo)
{
	long	i;
	long	j;

	for (i = 0; i < s->iters; i++)
	{
		for (j = 0; j < s->n; j++)
		{
			g_buf[j] = al->mal(s->a);
			if (g_buf[j])
				((char *)g_buf[j])[0] = 1;
		}
		j = lifo ? s->n - 1 : 0;
		while (j >= 0 && j < s->n)
		{
			al->fre(g_buf[j]);
			j += lifo ? -1 : 1;
		}
	}
	return (s->iters * s->n * 2);
}

static long	do_calloc(const t_alloc *al, const t_scn *s)
{
	long	i;
	char	*p;

	for (i = 0; i < s->iters; i++)
	{
		p = al->cal((size_t)s->n, s->a);
		if (p)
		{
			g_sink += (unsigned long)(unsigned char)p[0];
			al->fre(p);
		}
	}
	return (s->iters);
}

static long	do_realloc(const t_alloc *al, const t_scn *s)
{
	long	i;
	long	j;
	size_t	sz;
	char	*p;

	for (i = 0; i < s->iters; i++)
	{
		sz = s->a;
		p = al->mal(sz);
		for (j = 0; j < s->n; j++)
		{
			sz = sz * 2;
			p = al->rea(p, sz);
			if (p)
				p[sz - 1] = (char)j;
		}
		g_sink += (unsigned long)(size_t)p;
		al->fre(p);
	}
	return (s->iters * s->n);
}

static long	do_memalign(const t_alloc *al, const t_scn *s)
{
	long	i;
	char	*p;

	for (i = 0; i < s->iters; i++)
	{
		p = al->ali(s->b, s->a);
		if (p)
		{
			p[0] = 1;
			g_sink += (unsigned long)(unsigned char)p[0];
			al->fre(p);
		}
	}
	return (s->iters);
}

static long	do_mixed(const t_alloc *al, const t_scn *s)
{
	long			i;
	unsigned int	seed;
	size_t			sz;
	long			slot;

	seed = 0x1234567u;
	memset(g_buf, 0, sizeof(g_buf));
	for (i = 0; i < s->iters; i++)
	{
		sz = s->a + (xrand(&seed) % (s->b - s->a + 1));
		slot = xrand(&seed) % s->n;
		if (g_buf[slot])
		{
			al->fre(g_buf[slot]);
			g_buf[slot] = NULL;
		}
		else
		{
			g_buf[slot] = al->mal(sz);
			if (g_buf[slot])
				((char *)g_buf[slot])[0] = 1;
		}
	}
	for (slot = 0; slot < s->n; slot++)
		if (g_buf[slot])
			al->fre(g_buf[slot]);
	return (s->iters);
}

static long	run_scn(const t_alloc *al, const t_scn *s)
{
	if (s->kind == K_CHURN)
		return (do_churn(al, s));
	if (s->kind == K_BATCH_LIFO)
		return (do_batch(al, s, 1));
	if (s->kind == K_BATCH_FIFO)
		return (do_batch(al, s, 0));
	if (s->kind == K_CALLOC)
		return (do_calloc(al, s));
	if (s->kind == K_REALLOC)
		return (do_realloc(al, s));
	if (s->kind == K_MEMALIGN)
		return (do_memalign(al, s));
	return (do_mixed(al, s));
}

static double	timeit(const t_alloc *al, const t_scn *s, long *ops)
{
	int		r;
	double	best;
	double	t0;
	double	dt;

	*ops = run_scn(al, s);
	best = 1e18;
	for (r = 0; r < REPS; r++)
	{
		t0 = now_ns();
		*ops = run_scn(al, s);
		dt = now_ns() - t0;
		if (dt < best)
			best = dt;
	}
	return (best);
}

static const t_alloc	g_ft = {
	"ft", ft_malloc, ft_free, ft_realloc, ft_calloc, ft_memalign
};
static const t_alloc	g_libc = {
	"libc", malloc, free, realloc, calloc, libc_memalign
};

#include "bench_scn.h"

static void	report_row(const t_scn *s, double ftt, double lct, long ops)
{
	double		ftns;
	double		lcns;
	const char	*win;
	double		ratio;

	ftns = ftt / (double)ops;
	lcns = lct / (double)ops;
	if (ftt <= lct)
	{
		win = "ft";
		ratio = lct / ftt;
	}
	else
	{
		win = "libc";
		ratio = ftt / lct;
	}
	printf("%-26s %10.1f %10.1f   %-5s %5.2fx\n",
		s->name, ftns, lcns, win, ratio);
}

int	main(void)
{
	long	i;
	long	ops;
	double	ftt;
	double	lct;
	int		ftw;
	int		lcw;
	double	gsum;

	printf("ft_malloc vs libc  (%d scenarios, best of %d, ns per op)\n\n",
		(int)(sizeof(g_scn) / sizeof(g_scn[0])), REPS);
	printf("%-26s %10s %10s   %-5s %s\n",
		"scenario", "ft", "libc", "win", "x");
	printf("%-26s %10s %10s   %-5s %s\n",
		"--------", "--", "----", "---", "-");
	ftw = 0;
	lcw = 0;
	gsum = 0.0;
	i = -1;
	while (++i < (long)(sizeof(g_scn) / sizeof(g_scn[0])))
	{
		ftt = timeit(&g_ft, &g_scn[i], &ops);
		lct = timeit(&g_libc, &g_scn[i], &ops);
		report_row(&g_scn[i], ftt, lct, ops);
		if (ftt <= lct)
			ftw++;
		else
			lcw++;
		gsum += log(lct / ftt);
	}
	printf("\nsummary: ft faster in %d, libc faster in %d  (of %d)\n",
		ftw, lcw, ftw + lcw);
	printf("geomean speedup ft/libc: %.3fx  (>1 = ft faster overall)\n",
		exp(gsum / (double)(ftw + lcw)));
	return (0);
}

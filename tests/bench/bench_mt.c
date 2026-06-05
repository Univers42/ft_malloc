/*
 * Multithreaded throughput: ft_malloc vs libc at 1..16 threads.
 * Each thread runs an independent malloc/touch/free churn over small size
 * classes (the scalable common case: ft's per-thread magazine is lock-free, so
 * it should scale like glibc's tcache). We report Mops/s and the ft/libc
 * speedup per thread count, plus the geomean. ft and libc coexist in one
 * process (ft does not interpose libc).
 */
#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

typedef void	*(*t_mfn)(size_t);
typedef void	(*t_ffn)(void *);

static t_mfn			g_mal;
static t_ffn			g_fre;
static long				g_iters;
static pthread_barrier_t	g_start;

static unsigned int	xr(unsigned int *s)
{
	*s ^= *s << 13;
	*s ^= *s >> 17;
	*s ^= *s << 5;
	return (*s);
}

static void	*worker(void *arg)
{
	unsigned int	seed;
	long			i;
	size_t			n;
	unsigned char	*p;

	seed = (unsigned int)(unsigned long)arg | 1u;
	pthread_barrier_wait(&g_start);
	i = -1;
	while (++i < g_iters)
	{
		n = (size_t)(xr(&seed) % 1024) + 1;
		p = g_mal(n);
		if (p)
		{
			p[0] = (unsigned char)i;
			p[n - 1] = (unsigned char)n;
			g_fre(p);
		}
	}
	return (NULL);
}

static double	now_s(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((double)ts.tv_sec + (double)ts.tv_nsec / 1e9);
}

static double	run(int nt)
{
	pthread_t	th[64];
	int			i;
	double		t0;
	double	dt;

	pthread_barrier_init(&g_start, NULL, (unsigned)nt);
	i = -1;
	while (++i < nt)
		pthread_create(&th[i], NULL, worker, (void *)(long)(i + 1));
	t0 = now_s();
	i = -1;
	while (++i < nt)
		pthread_join(th[i], NULL);
	dt = now_s() - t0;
	pthread_barrier_destroy(&g_start);
	return ((double)nt * (double)g_iters / dt / 1e6);
}

static double	measure(int nt, t_mfn m, t_ffn f)
{
	double	best;
	int		r;
	double	v;

	g_mal = m;
	g_fre = f;
	best = 0;
	r = -1;
	while (++r < 3)
	{
		v = run(nt);
		if (v > best)
			best = v;
	}
	return (best);
}

int	main(void)
{
	int		tc[5];
	int		i;
	double	ft;
	double	lc;
	double	gsum;

	tc[0] = 1;
	tc[1] = 2;
	tc[2] = 4;
	tc[3] = 8;
	tc[4] = 16;
	g_iters = 1000000;
	printf("ft_malloc vs libc -- multithreaded throughput (Mops/s)\n\n");
	printf("%-8s %12s %12s   %s\n", "threads", "ft", "libc", "ft/libc");
	gsum = 0.0;
	i = -1;
	while (++i < 5)
	{
		ft = measure(tc[i], ft_malloc, ft_free);
		lc = measure(tc[i], malloc, free);
		printf("%-8d %12.1f %12.1f   %.2fx\n", tc[i], ft, lc, ft / lc);
		gsum += log(ft / lc);
	}
	printf("\ngeomean ft/libc across thread counts: %.3fx\n", exp(gsum / 5.0));
	return (0);
}

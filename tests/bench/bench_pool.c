/*
 * Concurrent server-pool benchmark (Larson-style) -- the canonical test for
 * multithreaded allocators, and a realistic "real program": a fixed array of
 * live connection/request buffers churned by a pool of worker threads.
 *
 * Each step a worker mallocs a new small buffer, atomically swaps it into a
 * random slot, and frees whatever buffer was there before -- which was almost
 * always allocated by a DIFFERENT thread. That cross-thread free is the hard
 * case real servers impose (a request accepted on one thread, its buffer freed
 * on another). It is pure malloc/free (no memcpy to dilute the allocator), so
 * it isolates allocator throughput under cross-thread pressure.
 *
 * ft handles this lock-free: every block self-describes its size class in the
 * header, so a free by any thread goes to that thread's magazine without
 * touching the owner. glibc must reach the owning arena / its slower remote
 * path. Built twice: default => ft_malloc, -DMODE_MALLOC => libc.
 * `make bench-pool` runs both at 1/2/4/8/16 threads and prints the speedup.
 */
#include "alloc.h"
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <math.h>

#ifdef MODE_MALLOC
# define ALLOC malloc
# define FREE free
# define ANAME "libc"
#else
# define ALLOC ft_malloc
# define FREE ft_free
# define ANAME "ft_malloc"
#endif

#define NSLOTS 8192

static _Atomic(void *)		g_slots[NSLOTS];
static long					g_iters;
static pthread_barrier_t	g_start;

static unsigned int	xr(unsigned int *s)
{
	*s ^= *s << 13;
	*s ^= *s >> 17;
	*s ^= *s << 5;
	return (*s);
}

/* small-object-heavy mix, like real request buffers (16..255, sometimes ~1KB) */
static size_t	pick_size(unsigned int *s)
{
	if ((xr(s) & 7) == 0)
		return (256 + (xr(s) % 768));
	return (16 + (xr(s) % 240));
}

static double	now_s(void)
{
	struct timespec	t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return ((double)t.tv_sec + (double)t.tv_nsec / 1e9);
}

static void	*worker(void *arg)
{
	unsigned int	seed;
	long			i;
	size_t			sz;
	unsigned char	*p;
	void			*old;

	seed = (unsigned int)(unsigned long)arg * 2654435761u | 1u;
	pthread_barrier_wait(&g_start);
	i = -1;
	while (++i < g_iters)
	{
		sz = pick_size(&seed);
		p = ALLOC(sz);
		if (p)
		{
			p[0] = (unsigned char)i;
			p[sz - 1] = (unsigned char)sz;
		}
		old = atomic_exchange(&g_slots[xr(&seed) % NSLOTS], (void *)p);
		if (old)
			FREE(old);
	}
	return (NULL);
}

static void	drain_slots(void)
{
	int		i;
	void	*old;

	i = -1;
	while (++i < NSLOTS)
	{
		old = atomic_exchange(&g_slots[i], NULL);
		if (old)
			FREE(old);
	}
}

static double	run(int nt)
{
	pthread_t	th[64];
	int			i;
	double		t0;
	double		dt;

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
	drain_slots();
	return (dt);
}

static double	measure(int nt)
{
	double	best;
	double	dt;
	int		r;

	best = 1e18;
	r = -1;
	while (++r < 3)
	{
		dt = run(nt);
		if (dt < best)
			best = dt;
	}
	return (best);
}

int	main(void)
{
	int		tc[5];
	int		i;
	double	mops;

	tc[0] = 1;
	tc[1] = 2;
	tc[2] = 4;
	tc[3] = 8;
	tc[4] = 16;
	g_iters = 2000000;
	printf("%-9s %8s %14s\n", ANAME, "threads", "Mops/s");
	i = -1;
	while (++i < 5)
	{
		mops = (double)tc[i] * (double)g_iters / measure(tc[i]) / 1e6;
		printf("%-9s %8d %14.2f\n", ANAME, tc[i], mops);
	}
	return (0);
}

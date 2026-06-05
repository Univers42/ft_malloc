/*
 * Real-program MULTITHREADED malloc+realloc benchmark, get_next_line-style.
 *
 * Each thread reads the whole file with a self-contained line reader that
 * rebuilds every line in a freshly malloc'd buffer grown by repeated realloc
 * (doubling) and freed at end-of-line -- exactly get_next_line's allocation
 * pattern, but with no shared/static state, so T threads can run it safely and
 * hammer the allocator concurrently. (get_next_line itself keeps per-fd static
 * scan state, which is not safe once threads recycle fd numbers.)
 *
 * Built twice: default => ft_malloc, -DMODE_MALLOC => libc. `make bench-gnl-mt`
 * runs both at 1/2/4/8 threads and prints the per-thread-count speedup.
 */
#include "get_next_line.h"
#include <pthread.h>
#include <time.h>

#ifdef MODE_MALLOC
# define ANAME "libc"
#else
# define ANAME "ft_malloc"
#endif

static const char			*g_path;
static int					g_passes;
static pthread_barrier_t	g_start;
static long					g_lines[64];

static double	now_s(void)
{
	struct timespec	t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return ((double)t.tv_sec + (double)t.tv_nsec / 1e9);
}

/* get_next_line's exact per-line pattern, static-free: read a BUFFER_SIZE
 * chunk, append up to and including '\n' in one ensure_cap (one doubling
 * realloc to fit), emit the line and free it. ~1 realloc per line, like gnl. */
static long	read_file_lines(const char *path)
{
	int		fd;
	char	rb[BUFFER_SIZE];
	char	*line;
	size_t	cap;
	size_t	len;
	size_t	chunk;
	size_t	need;
	ssize_t	n;
	ssize_t	off;
	long	lines;
	char	*nl;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	line = NULL;
	cap = 0;
	len = 0;
	lines = 0;
	while ((n = read(fd, rb, sizeof(rb))) > 0)
	{
		off = 0;
		while (off < n)
		{
			nl = memchr(rb + off, '\n', (size_t)(n - off));
			if (nl)
				chunk = (size_t)(nl - (rb + off) + 1);
			else
				chunk = (size_t)(n - off);
			need = len + chunk + 1;
			if (need > cap)
			{
				while (cap < need)
					cap = cap ? cap * 2 : 64;
				line = GNL_REALLOC(line, cap);
			}
			memcpy(line + len, rb + off, chunk);
			len += chunk;
			line[len] = '\0';
			off += (ssize_t)chunk;
			if (nl)
			{
				lines++;
				GNL_FREE(line);
				line = NULL;
				cap = 0;
				len = 0;
			}
		}
	}
	if (len)
		lines++;
	GNL_FREE(line);
	close(fd);
	return (lines);
}

static void	*worker(void *arg)
{
	long	idx;
	int		p;
	long	n;

	idx = (long)arg;
	n = 0;
	pthread_barrier_wait(&g_start);
	p = -1;
	while (++p < g_passes)
		n += read_file_lines(g_path);
	g_lines[idx] = n;
	return (NULL);
}

static double	run(int nt, long *out_lines)
{
	pthread_t	th[64];
	long		i;
	double		t0;
	double		dt;

	pthread_barrier_init(&g_start, NULL, (unsigned)nt);
	i = -1;
	while (++i < nt)
		pthread_create(&th[i], NULL, worker, (void *)i);
	t0 = now_s();
	i = -1;
	while (++i < nt)
		pthread_join(th[i], NULL);
	dt = now_s() - t0;
	pthread_barrier_destroy(&g_start);
	*out_lines = 0;
	i = -1;
	while (++i < nt)
		*out_lines += g_lines[i];
	return (dt);
}

/* best (lowest) time of several runs, to damp scheduler/memcpy noise */
static double	measure(int nt, long *out_lines)
{
	double	best;
	double	dt;
	int		r;

	best = 1e18;
	r = -1;
	while (++r < 5)
	{
		dt = run(nt, out_lines);
		if (dt < best)
			best = dt;
	}
	return (best);
}

static void	gen_file(const char *path, long target)
{
	int				fd;
	unsigned int	s;
	long			w;
	char			line[512];
	int				i;
	int				len;

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	s = 2463534242u;
	w = 0;
	while (w < target)
	{
		s = s * 1103515245u + 12345u;
		len = 8 + (int)((s >> 8) % 400);
		i = -1;
		while (++i < len)
			line[i] = (char)('a' + ((s >> (i & 15)) % 26));
		line[len] = '\n';
		if (write(fd, line, (size_t)len + 1) < 0)
			break ;
		w += len + 1;
	}
	close(fd);
}

int	main(void)
{
	int		tc[5];
	int		i;
	long	lines;
	double	dt;

	tc[0] = 1;
	tc[1] = 2;
	tc[2] = 4;
	tc[3] = 8;
	tc[4] = 16;
	g_path = "/tmp/ft_gnl_mt.txt";
	g_passes = 8;
	gen_file(g_path, 8L * 1024 * 1024);
	printf("%-9s %8s %12s %14s\n", ANAME, "threads", "time(s)", "Mlines/s");
	i = -1;
	while (++i < 5)
	{
		dt = measure(tc[i], &lines);
		printf("%-9s %8d %12.4f %14.2f\n", ANAME, tc[i], dt,
			(double)lines / dt / 1e6);
	}
	return (0);
}

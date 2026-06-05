/*
 * Real-program malloc+realloc benchmark: get_next_line.
 *
 * get_next_line is malloc/realloc-heavy: every line is built in a dynamically
 * grown buffer (GNL_MALLOC + repeated GNL_REALLOC via ensure_cap) and handed to
 * the caller to GNL_FREE. We run the *actual* get_next_line.c over a generated
 * file, with no printf in the hot loop (so we measure the allocator, not I/O).
 *
 * Built twice from the same source: default => ft_malloc, -DMODE_MALLOC => libc.
 * `make bench-gnl` runs both and prints the speedup.
 */
#include "get_next_line.h"
#include <time.h>

#ifdef MODE_MALLOC
# define ANAME "libc"
#else
# define ANAME "ft_malloc"
#endif

static double	now_s(void)
{
	struct timespec	t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return ((double)t.tv_sec + (double)t.tv_nsec / 1e9);
}

/* Generate a file of pseudo-random-length lines (8..407 bytes) ~ target bytes. */
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

static double	run_once(const char *path, long *lines, unsigned long *bytes)
{
	int				fd;
	char			*l;
	double			t0;
	long			n;
	unsigned long	b;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (-1.0);
	t0 = now_s();
	n = 0;
	b = 0;
	while ((l = get_next_line(fd)) != NULL)
	{
		b += (unsigned long)strlen(l);
		n++;
		GNL_FREE(l);
	}
	close(fd);
	*lines = n;
	*bytes = b;
	return (now_s() - t0);
}

int	main(int argc, char **argv)
{
	const char		*path = "/tmp/ft_gnl_bench.txt";
	int				reps;
	int				r;
	double			best;
	double			dt;
	long			lines;
	unsigned long	bytes;

	reps = 7;
	if (argc > 1)
		reps = atoi(argv[1]);
	gen_file(path, 16L * 1024 * 1024);
	best = 1e18;
	lines = 0;
	bytes = 0;
	r = -1;
	while (++r < reps)
	{
		dt = run_once(path, &lines, &bytes);
		if (dt >= 0 && dt < best)
			best = dt;
	}
	printf("%-9s lines=%ld bytes=%lu  time=%.4fs  %.2f Mlines/s  %.0f MB/s\n",
		ANAME, lines, bytes, best, (double)lines / best / 1e6,
		(double)bytes / best / 1e6);
	return (0);
}

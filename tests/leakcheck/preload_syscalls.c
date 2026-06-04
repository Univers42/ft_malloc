/*
 * preload_syscalls.c  --  Lane 1 syscall interposer for the ft_malloc audit.
 *
 * LD_PRELOAD this shim in front of the harness. It wraps mmap/munmap/mremap/
 * sbrk via dlsym(RTLD_NEXT), records every mmap region, and reports at exit (and
 * on demand via the weak hooks below) which regions are still mapped, plus net
 * brk movement and call counters. ft_malloc's .so calls these through the PLT,
 * so no rebuild of the allocator is needed.
 *
 * Output uses raw write(2) (no printf) because glibc printf/malloc allocate via
 * mmap/brk -- routing through our own wrappers -- so a recursion guard + manual
 * formatting keeps reporting from deadlocking or polluting the counts.
 *
 * The harness drives baseline/report through weak symbols:
 *     syscount_baseline()  - mark "everything mapped after here is suspect"
 *     syscount_report()    - dump regions mapped since baseline + counters
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <sys/mman.h>

#define MAXREG 16384

typedef struct s_region
{
	void	*addr;
	size_t	len;
	int		live;
}	t_region;

static t_region			g_reg[MAXREG];
static int				g_nreg;
static int				g_baseline_idx;
static unsigned long	g_nmmap;
static unsigned long	g_nmunmap;
static unsigned long	g_nmremap;
static unsigned long	g_nsbrk;
static unsigned long	g_mmap_bytes;
static unsigned long	g_munmap_bytes;
static long				g_brk_delta;
static int				g_guard;

static void	*(*real_mmap)(void *, size_t, int, int, int, off_t);
static int	(*real_munmap)(void *, size_t);
static void	*(*real_mremap)(void *, size_t, size_t, int, ...);
static void	*(*real_sbrk)(intptr_t);

static void	resolve(void)
{
	if (!real_mmap)
		real_mmap = dlsym(RTLD_NEXT, "mmap");
	if (!real_munmap)
		real_munmap = dlsym(RTLD_NEXT, "munmap");
	if (!real_mremap)
		real_mremap = dlsym(RTLD_NEXT, "mremap");
	if (!real_sbrk)
		real_sbrk = dlsym(RTLD_NEXT, "sbrk");
}

/* ---- raw, malloc-free output helpers -------------------------------------- */
static void	put(const char *s)
{
	ssize_t	r;

	r = write(2, s, strlen(s));
	(void)r;
}

static void	put_ul(unsigned long v)
{
	char	b[32];
	int		i;
	ssize_t	r;

	i = 32;
	if (v == 0)
	{
		put("0");
		return ;
	}
	while (v && i > 0)
	{
		b[--i] = (char)('0' + (v % 10));
		v /= 10;
	}
	r = write(2, b + i, (size_t)(32 - i));
	(void)r;
}

static void	put_l(long v)
{
	if (v < 0)
	{
		put("-");
		put_ul((unsigned long)(-v));
	}
	else
		put_ul((unsigned long)v);
}

static void	put_ptr(void *p)
{
	char			b[16];
	int				i;
	uintptr_t		v;
	const char		*hex;
	ssize_t			r;

	hex = "0123456789abcdef";
	v = (uintptr_t)p;
	i = 16;
	while (i > 0)
	{
		b[--i] = hex[v & 0xf];
		v >>= 4;
	}
	put("0x");
	r = write(2, b, 16);
	(void)r;
}

/* ---- region bookkeeping --------------------------------------------------- */
static void	record_mmap(void *addr, size_t len)
{
	if (g_nreg >= MAXREG)
		return ;
	g_reg[g_nreg].addr = addr;
	g_reg[g_nreg].len = len;
	g_reg[g_nreg].live = 1;
	g_nreg++;
}

static void	record_munmap(void *addr)
{
	int	i;

	i = g_nreg;
	while (--i >= 0)
	{
		if (g_reg[i].live && g_reg[i].addr == addr)
		{
			g_reg[i].live = 0;
			g_munmap_bytes += g_reg[i].len;
			return ;
		}
	}
}

/* ---- public weak hooks (resolved only when preloaded) --------------------- */
void	syscount_baseline(void)
{
	g_baseline_idx = g_nreg;
	put("[syscount] baseline set after ");
	put_ul((unsigned long)g_nreg);
	put(" mmap regions\n");
}

unsigned long	syscount_live_since_baseline(void)
{
	int				i;
	unsigned long	n;

	n = 0;
	i = g_baseline_idx - 1;
	while (++i < g_nreg)
		if (g_reg[i].live)
			n++;
	return (n);
}

void	syscount_report(void)
{
	int	i;
	int	leaks;

	leaks = 0;
	put("\n=== syscall interposer report ===\n");
	put("mmap calls   : ");
	put_ul(g_nmmap);
	put("  bytes ");
	put_ul(g_mmap_bytes);
	put("\nmunmap calls : ");
	put_ul(g_nmunmap);
	put("  bytes ");
	put_ul(g_munmap_bytes);
	put("\nmremap calls : ");
	put_ul(g_nmremap);
	put("\nsbrk calls   : ");
	put_ul(g_nsbrk);
	put("  net brk delta ");
	put_l(g_brk_delta);
	put("\nregions still mapped since baseline:\n");
	i = g_baseline_idx - 1;
	while (++i < g_nreg)
	{
		if (g_reg[i].live)
		{
			put("  LIVE ");
			put_ptr(g_reg[i].addr);
			put("  ");
			put_ul((unsigned long)g_reg[i].len);
			put(" bytes\n");
			leaks++;
		}
	}
	put("  total live-since-baseline regions: ");
	put_ul((unsigned long)leaks);
	put("\n=================================\n");
}

__attribute__((destructor))
static void	syscount_atexit(void)
{
	syscount_report();
}

/* ---- the wrappers --------------------------------------------------------- */
void	*mmap(void *a, size_t len, int prot, int flags, int fd, off_t off)
{
	void	*r;

	resolve();
	r = real_mmap(a, len, prot, flags, fd, off);
	if (r != MAP_FAILED && !g_guard)
	{
		g_guard = 1;
		record_mmap(r, len);
		g_nmmap++;
		g_mmap_bytes += len;
		g_guard = 0;
	}
	return (r);
}

int	munmap(void *a, size_t len)
{
	resolve();
	if (!g_guard)
	{
		g_guard = 1;
		record_munmap(a);
		g_nmunmap++;
		g_guard = 0;
	}
	return (real_munmap(a, len));
}

void	*mremap(void *old, size_t osz, size_t nsz, int flags, ...)
{
	void	*r;
	void	*na;
	va_list	ap;

	resolve();
	g_nmremap++;
	if (flags & MREMAP_FIXED)
	{
		va_start(ap, flags);
		na = va_arg(ap, void *);
		va_end(ap);
		r = real_mremap(old, osz, nsz, flags, na);
	}
	else
		r = real_mremap(old, osz, nsz, flags);
	return (r);
}

void	*sbrk(intptr_t inc)
{
	resolve();
	if (!g_guard)
	{
		g_guard = 1;
		g_nsbrk++;
		g_brk_delta += (long)inc;
		g_guard = 0;
	}
	return (real_sbrk(inc));
}

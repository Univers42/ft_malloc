/*
 * No-op replacement for src/debug/track.c, linked into the benchmark build only.
 * The shipped allocator runs an allocation tracker (powering show_alloc_mem) on
 * every malloc/free; libc has no equivalent and the tracker's high-water scan is
 * O(n), so including it would measure the tracker, not the allocator. The bench
 * filters out track.c and links this instead to compare the pure allocator vs
 * libc fairly. (`make bench-shipped` keeps the tracker, to quantify its cost.)
 */
#include <stddef.h>

void	track_allocation_dbg(void *ptr, size_t size, const char *file, int line)
{
	(void)ptr;
	(void)size;
	(void)file;
	(void)line;
}

void	track_allocation(void *ptr, size_t size)
{
	(void)ptr;
	(void)size;
}

void	untrack_allocation(void *ptr)
{
	(void)ptr;
}

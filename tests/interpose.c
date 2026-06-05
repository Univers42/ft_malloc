/*
 * LD_PRELOAD shim: route a normal program's libc malloc/free/realloc/calloc to
 * ft_malloc, without recompiling that program. ft_malloc is NOT an interposer
 * (its public symbols are ft_*-prefixed), so we provide the libc-named symbols
 * here and forward them. Used to run the dining-philosophers program (and any
 * other dynamically-linked binary) entirely on ft_malloc:
 *
 *   make philo-ft   # builds bin/libft_preload.so and runs philo on ft_malloc
 *   LD_PRELOAD=$PWD/bin/libft_preload.so ./some_program ...
 *
 * ft_malloc grows its arenas with mmap/sbrk directly (no libc malloc), so there
 * is no recursion. Every allocation in the process is served by ft from the
 * first call, so frees always match.
 */
#include "alloc.h"

void	*malloc(size_t size)
{
	return (ft_malloc(size));
}

void	free(void *ptr)
{
	ft_free(ptr);
}

void	*realloc(void *ptr, size_t size)
{
	return (ft_realloc(ptr, size));
}

void	*calloc(size_t nmemb, size_t size)
{
	return (ft_calloc(nmemb, size));
}

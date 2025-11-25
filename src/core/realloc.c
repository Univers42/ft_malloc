#include "alloc.h"

/*
 * This translation unit intentionally does not define internal_realloc.
 * The full implementation lives in src/core/private/internal_realloc.c
 * to avoid multiple-definition link errors.
 *
 * Public wrapper ft_realloc is provided in suite_alloc.c (or calls
 * internal_realloc directly), so no symbols are needed here.
 */

/* public wrapper: forward to the real implementation in the private TU */
t_addr
ft_realloc(t_addr mem, size_t nbytes)
{
    ensure_init();
    return (internal_realloc(mem, nbytes, (char *)NULL, 0, 0));
}

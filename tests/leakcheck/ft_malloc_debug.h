/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc_debug.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Optional call-site instrumentation layer for the leak audit harness.
 *
 * The allocator already threads (file, line) through internal_malloc/realloc,
 * so these thin wrappers capture the caller's __FILE__/__LINE__ and store it in
 * the allocation tracker. A test that does:
 *
 *     #define FT_MALLOC_DEBUG
 *     #include "ft_malloc_debug.h"
 *
 * gets ft_malloc/ft_realloc/... transparently routed through the *_dbg variants,
 * so malloc_leak_map() can report the originating site of every live block.
 *
 * Without FT_MALLOC_DEBUG this header only declares the wrappers; the public
 * API and the shipped library build are untouched.
 */

#ifndef FT_MALLOC_DEBUG_H
# define FT_MALLOC_DEBUG_H

# include "public.h"

t_addr	ft_malloc_dbg(size_t size, const char *file, int line);
t_addr	ft_realloc_dbg(t_addr mem, size_t nbytes, const char *file, int line);
void	ft_free_dbg(t_addr mem, const char *file, int line);
t_addr	ft_calloc_dbg(size_t n, size_t s, const char *file, int line);
t_addr	ft_valloc_dbg(size_t size, const char *file, int line);
t_addr	ft_memalign_dbg(size_t align, size_t size, const char *file, int line);
void	ft_cfree_dbg(t_addr mem, const char *file, int line);

# ifdef FT_MALLOC_DEBUG
#  define ft_malloc(s) ft_malloc_dbg((s), __FILE__, __LINE__)
#  define ft_realloc(m, s) ft_realloc_dbg((m), (s), __FILE__, __LINE__)
#  define ft_free(m) ft_free_dbg((m), __FILE__, __LINE__)
#  define ft_calloc(n, s) ft_calloc_dbg((n), (s), __FILE__, __LINE__)
#  define ft_valloc(s) ft_valloc_dbg((s), __FILE__, __LINE__)
#  define ft_memalign(a, s) ft_memalign_dbg((a), (s), __FILE__, __LINE__)
#  define ft_cfree(m) ft_cfree_dbg((m), __FILE__, __LINE__)
# endif

#endif

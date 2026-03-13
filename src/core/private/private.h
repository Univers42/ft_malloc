/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   private.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:51:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/14 00:00:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRIVATE_H
# define PRIVATE_H

# include "alloc.h"
# include <string.h>

typedef struct s_val_ctx
{
	t_addr		mem;
	const char	*file;
	int			line;
}	t_val_ctx;

typedef struct s_ifree_ctx
{
	t_addr		mem;
	t_mhead		*p;
	char		*ap;
	int			nunits;
	uint32_t	nbytes;
	uint32_t	ubytes;
	t_glob		*g;
	int			freed;
}	t_ifree_ctx;

typedef struct s_mremap_params
{
	t_addr		mem;
	size_t		n;
	int			newunits;
	uint32_t	tocopy;
	int			flags;
	const char	*file;
	int			line;
	t_glob		*g;
	int			nunits;
}	t_mremap_params;

typedef struct s_alloc_ctx
{
	t_addr		ptr;
	t_mhead		*p;
	size_t		n;
	int			nunits;
	int			flags;
	const char	*file;
	int			line;
	t_glob		*g;
}	t_alloc_ctx;

void		validate_end_guard(char *ap, t_mhead *p, t_val_ctx *v);
int			handle_special_free_cases(t_mhead *p, int nu, t_glob *g);
int			try_munmap_free(t_mhead *p, int nunits, t_glob *g);
int			try_lesscore_free(t_mhead *p, int nunits, t_glob *g);
void		add_to_free_list(t_mhead *p, int nunits, t_glob *g);
void		handle_busy_bucket(t_mhead *p, int nunits, t_glob *g);
void		trace_and_watch_setup(t_mhead *p, uint32_t *ubytes);
void		handle_memalign(t_addr *mem, char **ap, t_mhead **p);
void		validate_alloc_status(t_addr mem, t_mhead *p, t_val_ctx *v);
void		validate_magic8(t_mhead *p, t_val_ctx *v);
void		ifree_prepare(t_ifree_ctx *c, t_val_ctx *v);
void		ifree_handle_free(t_ifree_ctx *c);
int			calculate_nunits(uint32_t nbytes, t_glob *g);
int			handle_reentrant(int nunits, t_glob *g);
t_mhead		*get_block_from_freelist(int nunits, t_glob *g);
void		validate_free_block(t_mhead *p, int nu, t_val_ctx *v);
void		setup_block_header(t_mhead *p, size_t n);
void		setup_end_guard(t_mhead *p, size_t n);
void		scramble_allocated_memory(void *mem, size_t n);
void		update_recurse_stats(t_glob *g, int nunits);
void		update_alloc_stats(t_glob *g, int nunits, size_t n);
void		handle_malloc_trace_alloc(t_alloc_ctx *ctx);
void		handle_malloc_register_alloc(t_alloc_ctx *ctx);
void		handle_malloc_watch_alloc(t_alloc_ctx *ctx);
void		check_alignment(t_alloc_ctx *ctx);
int			check_max_alloc_size(size_t n);
int			should_trace_or_register_or_watch(void);
t_addr		mremap_implement(t_mremap_params *params);
t_addr		internal_malloc(size_t n, const char *file,
				int line, int flags);
t_addr		internal_free(t_addr mem, const char *file,
				int line, int flags);
t_addr		internal_realloc(t_addr mem, size_t n,
				t_val_ctx *v, int flags);
t_addr		internal_memalign(size_t align, size_t size,
				t_val_ctx *v, int flags);
t_addr		internal_valloc(size_t size, t_val_ctx *v, int flags);
t_addr		internal_calloc(size_t n, size_t s,
				const char *file, int line);
void		internal_cfree(t_addr p, const char *file,
				int line, int flags);
int			posix_memalign(void **memptr, size_t align, size_t sz);
t_addr		internal_remap(t_addr mem, size_t n, int nu, int fl);

#endif

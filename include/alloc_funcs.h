/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc_funcs.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:10:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 21:24:49 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLOC_FUNCS_H
# define ALLOC_FUNCS_H

# include "alloc_types.h"

t_addr		ft_malloc(size_t size);
t_addr		ft_realloc(t_addr mem, size_t nbytes);
void		ft_free(t_addr mem);
t_addr		ft_memalign(size_t align, size_t size);
t_addr		ft_calloc(size_t n, size_t s);
void		ft_cfree(t_addr mem);

int			mover_head(void);
size_t		allocated_bytes(size_t n);
t_mhead		*chain(t_mhead *a);
t_mhead		**chain_ptr(t_mhead *a);
void		malloc_stats_sbrk_update(long sbrk_needed);
int			pagealign(void);
void		botch(const char *s, const char *file, int line);
void		xbotch(int e, const char *s, const char *file, int line);
void		bcoalesce(int nu);

t_glob		*get_glob(t_glob_field field, void *value);
void		init_allocator_glob(void);
void		bsplit(int nu);
void		xsplit(t_mhead *mp, int nu);
bool		in_bucket(size_t nb, int nu);
bool		right_bucket(size_t nb, int nu);
void		compute_stats_core(t_glob *g, int nu, int flag, size_t v);
void		compute_stats_brk(t_glob *g, size_t value);
void		compute_stats_mmap(t_glob *g, size_t value);
void		compute_stats_realloc_copy(t_glob *g);
int			is_powerof2(int x);
void		assert_or_abort(int cond, const char *expr,
				const char *file, int line);

void		malloc_memset(void *charp, int xch, size_t nbytes);
void		malloc_bzero(void *charp, size_t nbytes);
void		malloc_zero(void *charp, size_t nbytes);
void		malloc_memcpy(void *dest, const void *src, size_t nbytes);
void		fastcopy(const void *s, void *d, size_t n);
size_t		malloc_usable_size(void *mem);

uint64_t	binsize(int x);
uint64_t	maxalloc_size(void);
void		lesscore(int nu);
void		morecore(int nu);

void		malloc_block_signals(sigset_t *setp, sigset_t *osetp);
void		malloc_unblock_signals(sigset_t *setp, sigset_t *osetp);

void		show_alloc_mem(void);
void		track_allocation(void *ptr, size_t size);
void		untrack_allocation(void *ptr);

t_zone		*create_zone(size_t block_size, size_t zone_size);
t_zone		*get_zone_with_space(t_zone **zl, size_t bs, size_t zs);
void		*allocate_from_zone(t_zone *zone);
void		free_to_zone(t_zone *zone, t_mhead *block);
t_zone		*find_zone_for_ptr(t_zone *zone_list, void *ptr);
void		set_state_mem(t_addr mem);
t_addr		get_state_mem(void);
void		bsplit_stats_inc(int nbuck);
void		morecore_stat_nmorecore(int nu, t_glob *g);
void		morecore_stat_nsbrk(long s, t_glob *g);
void		ensure_init(void);
int			get_page_size(void);

#endif

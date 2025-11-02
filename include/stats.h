/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stats.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:58:57 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:29:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATS_H
#define STATS_H

typedef enum e_stats
{
	STAT_LESS_CORE,
	STAT_MORE_CORE,
	STAT_MMAP,
	STAT_SBRK,
	STAT_BRK,
	STAT_REALLOC
} t_flag_stat;

#ifdef MALLOC_STATS
#ifndef NBUCKETS
#define NBUCKETS 28
#endif

typedef struct s_alloc_stats
{
	int nalloc[NBUCKETS];
	int tmalloc[NBUCKETS];
	int nmorecore[NBUCKETS];
	int nlesscore[NBUCKETS];
	int nmal;
	int nfre;
	int nrcopy;
	int nrealloc;
	int nrecurse;
	int nsbrk;
	int32_t tsbrk;
	int32_t byteused;
	uint32_t bytesreq;
	int tbsplit;
	int nsplit[NBUCKETS];
	int tbcoalesce;
	int ncoalesce[NBUCKETS];
	int nmmap;
	int32_t tmmap;
} t_stats;

typedef struct s_bucket_stats
{
	uint32_t blocksize;
	int nfree;
	int nused;
	int nmal;
	int nmorecore;
	int nlesscore;
	int nsplit;
	int ncoalesce;
	int nmmap;
} t_bucket_stats;

// extern struct bucket_stats malloc_bucket_stats (int);
// extern struct _malstats malloc_stats (void);
// extern void print_malloc_stats (char *);
// extern void trace_malloc_stats (char *, char *);
#endif /* MALLOC_STATS */

// void		compute_stats_core(t_glob *g, int nu, int flag, size_t value);

#endif /* MSTATS_H */
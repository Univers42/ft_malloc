/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stats.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:58:57 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 19:16:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATS_H
# define STATS_H

#include "imalloc.h"

#ifdef MALLOC_STATS
# ifndef NBUCKETS
#   define NBUCKETS 28
# endif

typedef struct s_alloc_stats
{
	int	nalloc[NBUCKETS];
	int	tmalloc[NBUCKETS];
	int	nmorecore[NBUCKETS];
	int	nlesscore[NBUCKETS];
	int	nmal;
	int	nfre;
	int	nrcopy;
	int	nrealloc;
	int	nrecurse;
	int	nsbrk;
	int32_t	tsbrk;
	int32_t	byteused;
	int32_t	bytesreq;
	uint32_t	bytesreq;
	int			tbsplit;
	int			nsplit[NBUCKETS];
	int			tbcoalesce;
	int			ncoalesce[NBUCKETS];
	int			nmmap;
	int32_t		tmmap;
}	t_alloc_stats;

typedef struct s_bucket_stats
{
	uint32_t	blocksize;
	int			nfree;
	int			nused;
	int			nmal;
	int			nmorecore;
	int			nlesscore;
	int			nsplit;
	int			ncoalesce;
	int			nmmap;
}	t_bucket_stats;

// extern struct bucket_stats malloc_bucket_stats (int);
// extern struct _malstats malloc_stats (void);
// extern void print_malloc_stats (char *);
// extern void trace_malloc_stats (char *, char *);
# endif	/* MALLOC_STATS */

#endif	/* MSTATS_H */
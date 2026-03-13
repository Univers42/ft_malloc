/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stats.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:58:57 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 21:31:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATS_H
# define STATS_H

# include <stdint.h>

# ifndef NBUCKETS
#  define NBUCKETS 28
# endif

typedef enum e_stats
{
	STAT_LESS_CORE,
	STAT_MORE_CORE,
	STAT_MMAP,
	STAT_SBRK,
	STAT_BRK,
	STAT_REALLOC
}	t_flag_stat;

typedef struct s_alloc_stats
{
	int			nalloc[NBUCKETS];
	int			tmalloc[NBUCKETS];
	int			nmorecore[NBUCKETS];
	int			nlesscore[NBUCKETS];
	int			nmal;
	int			nfre;
	int			nrcopy;
	int			nrealloc;
	int			nrecurse;
	int			nsbrk;
	int32_t		tsbrk;
	int32_t		byteused;
	uint32_t	bytesreq;
	int			tbsplit;
	int			nsplit[NBUCKETS];
	int			tbcoalesce;
	int			ncoalesce[NBUCKETS];
	int			nmmap;
	int32_t		tmmap;
}	t_stats;

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

#endif
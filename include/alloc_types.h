/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc_types.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:10:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/14 00:00:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLOC_TYPES_H
# define ALLOC_TYPES_H

# include <stdint.h>
# include <sys/types.h>
# include <stdbool.h>

# ifndef NBUCKETS
#  define NBUCKETS 28
# endif

typedef void		*t_addr;
typedef uint32_t	t_malloc_size;

typedef union u_malloc_guard
{
	char		s[4];
	uint32_t	i;
}	t_mguard;

typedef union u_mhead
{
	uint64_t	mh_align[2];
	struct s_minfo
	{
		uint8_t		mi_alloc;
		uint8_t		mi_index;
		uint16_t	mi_magic2;
		uint32_t	mi_nbytes;
		uint8_t		mi_magic8[8];
	}	s_minfo;
}	t_mhead;

typedef enum e_stmalloc
{
	MALLOC_WRAPPER = 0x01,
	MALLOC_INTERNAL = 0x02,
	MALLOC_NOTRACE = 0x04,
	MALLOC_NOREG = 0x08
}	t_stmalloc;

typedef enum e_err
{
	ERR_DUPFREE = 0x01,
	ERR_UNALLOC = 0x02,
	ERR_UNDERFLOW = 0x04,
	ERR_ASSERT_FAILED = 0x08
}	t_err;

typedef struct s_zone
{
	void			*start;
	void			*end;
	size_t			block_size;
	size_t			total_blocks;
	size_t			used_blocks;
	struct s_zone	*next;
	t_mhead			*free_list;
}	t_zone;

typedef struct s_glob
{
	int			pagesz;
	int			pagebucket;
	int			maxbuck;
	char		*memtop;
	char		busy[NBUCKETS];
	t_mhead		*nextf[NBUCKETS];
	uint64_t	binsizes[NBUCKETS];
	int			malloc_flags;
	int			malloc_trace;
	int			malloc_register;
	int			malloc_mmap_threshold;
	int			errn;
	t_zone		*tiny_zones;
	t_zone		*small_zones;
	void		*large_list;
}	t_glob;

typedef struct s_binspec
{
	const size_t	*bins;
	size_t			len;
}	t_binspec;

typedef enum e_glob_field
{
	GLOB_NONE = -1,
	GLOB_PAGESZ,
	GLOB_PAGEBUCKET,
	GLOB_MAXBUCK,
	GLOB_MEMTOP,
	GLOB_BINSIZE,
	GLOB_ERRN
}	t_glob_field;

typedef struct s_bcoalesce_ctx
{
	t_glob	*g;
	t_mhead	*mp1;
	t_mhead	*mp2;
	t_mhead	*mp;
	int		nbuck;
	int		nu;
}	t_bcoalesce_ctx;

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:10:09 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/31 18:03:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLOC_H
# define ALLOC_H
# include <stdbool.h>
# include <string.h>
# include <sys/types.h>
# include <signal.h>
# include <string.h>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>

# define MALLOC_PAGESIZE_MIN	4096
# define MALLOC_INCR_PAGES		8192

//usable bins from stacbuck...nbuck - 1
# undef HAVE_MMAP
# undef MALLOC_STATS
# undef MALLOC_REGISTER
# undef MALLOC_WATCH

# undef HAVE_GETPAGESIZE
# define NBUCKETS				28
# ifdef HAVE_MMAP
#  include <sys/mman.h>
# endif

typedef void*	t_addr;

/**
 * implementation of magic bytes that check
 * the correctheness of the datas
 */
# define ISALLOC		0xf7
# define ISFREE			0x54
/* Stored before the value returned by
memalign, with the rest of the word
being the distance to the true
beginning of the block.  */
# define ISMEMALILGN		0Xd6
# define MAGIC8_NUMBYTES	8
# define MALLOC_SIZE_T		u_bits32_t
# define MALIGN_MASK		15
# define MAGIC1				0x55
# define MAGIC2				0x5555
# define MSLOP				4
# define SPLIT_MIN			1
# define SPLIT_MID			9
# define SPLIT_MAX			12
# define COMBINE_MIN		1
# define SPLIT_
# define SPLIT_
typedef enum e_stmalloc
{
	MALLOC_WRAPPER = 0X01,
	MALLOC_INTERNAL = 0X02,
	MALLOC_NOTRACE = 0X04,
	MALLOC_NOREG = 0X08,
}	t_stmalloc;

typedef enum e_err
{
	ERR_DUPFREE = 0x01,
	ERR_UNALLOC = 0x02,
	ERR_UNDERFLOW = 0x04,
	ERR_ASSERT_FAILED = 0x08,
}	t_err;

/**
 * This union is used as a header for each allocated memory
 * block in a custom malloc implementation.
 * it stores metadata about the block (allocation status, idx,
 * size, guard bytes, etc.) for bookkeeping, error checking
 * and debugging.
 * Using a union with a struct is ideal because
 * the union allows us to enforce alignment for portability
 * and performmance.
 * The struct inside the union holds all the metadatas fileds
 * needed for our allocator (status, index, size, guard bytes)
 * It keeps the header size fixed and properly aligned, 
 * which is important for memory management and avoiding
 * undifined behavior.
 */
typedef union u_mhead
{
	uint64_t	mh_align[2];		//16
	struct
	{
		uint8_t		mi_alloc;		// ISALLOC OR ISFREE states
		uint8_t		mi_index;		// index in nextf[]
		uint16_t	mi_magic2;		// should be MAGIC2 // *2
		uint32_t	mi_nnbytes;		// # of bytes allocated // *4
		uint8_t		mi_nbytes8[8];	// MAGIC1 guard bytes // 8
	}s_minfo;
}	t_mhead;

typedef union	u_alloc_guard
{
	uint8_t		s[4];
	uint32_t	i;
}	t_mguard;

typedef struct s_glob
{
	int			pagesz;
	int			pagebucket;
	int			maxbuck;
	char		*maxbuck;
	char		busy[NBUCKETS];
	t_mhead		*nextf[NBUCKETS];
	uint64_t	binsizes[NBUCKETS];
	int			errn;
}	t_glob;


static inline int	mover_head(void)
{
	return (sizeof(t_mhead));
}

t_addr	malloc(size_t size);
t_addr	realloc (t_addr mem, size_t nbytes);
void	free(t_addr mem);
t_addr	ft_memalign(size_t	align, size_t size);
t_addr	valloc(size_t size);
t_addr	ft_calloc(size_t n, size_t s);
void	ft_cfree(t_addr mem);

#endif

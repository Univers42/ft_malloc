/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:10:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/14 00:30:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLOC_H
# define ALLOC_H

# include <stdbool.h>
# include <sys/types.h>
# include <signal.h>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>
# include <sys/mman.h>
# include <string.h>
# include "config.h"
# include "stats.h"
# include "alloc_types.h"
# include "alloc_funcs.h"

# define MALLOC_PAGESIZE_MIN 4096
# define MALLOC_INCR_PAGES 8192
# define NBUCKETS 28
# define ISALLOC 0xf7
# define ISFREE 0x54
# define ISMEMALILGN 0xD6
# define MAGIC8_NUMBYTES 8
# define MALIGN_MASK 15
# define MAGIC1 0x55
# define MAGIC2 0x5555
# define MSLOP 4
# define SPLIT_MIN 1
# define SPLIT_MID 9
# define SPLIT_MAX 12
# define COMBINE_MIN 1
# define LESSCORE_MIN 8
# define LESSCORE_FRC 11
# define PREPOP_BIN 1
# define PREPOP_SIZE 64
# define STARTBUCK 0

# ifdef HAVE_MMAP
#  ifdef MAP_ANON
#   ifndef MAP_ANONYMOUS
#    define MAP_ANONYMOUS MAP_ANON
#   endif
#  endif
# endif

# ifdef HAVE_MMAP
#  ifdef MAP_ANONYMOUS
#   define USE_MMAP 1
#  else
#   define USE_MMAP 0
#  endif
# else
#  define USE_MMAP 0
# endif

# define USE_MREMAP 0

# define MMAP_THRESHOLD 12

# ifdef USE_HYBRID_MODE
#  define USE_SBRK_FOR_TINY_SMALL 1
# else
#  define USE_SBRK_FOR_TINY_SMALL 0
# endif

# define TINY_MAX_SIZE 128
# define SMALL_MAX_SIZE 1024
# define TINY_ZONE_PAGES 4
# define SMALL_ZONE_PAGES 32
# define MIN_BLOCKS_PER_ZONE 100

# ifndef MAXALLOC_SIZE
#  define MAXALLOC_SIZE 0xFFFFFFFF
# endif

# ifndef SIZEOF_SIZE_T
#  define SIZEOF_SIZE_T 8
# endif

#endif

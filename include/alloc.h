/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:10:09 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:06:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLOC_H
#define ALLOC_H

/**in affair of allocations sometimes we need to chose the allocations the
 * quickest way possibly. Chosing the good data mgiration facilty is not easy
 * but as the pairs hae already thought to what to do in this case
 * they determine which function of setter of the heap could transmit more
 * quickly the maps..
 * mmap and munmap which are by far the modern way to set the heap are
 * terrible if allocation is low in units. So to derotate that we need to
 * discover the most quick possibly which to use between sbrk or those new
 * functions to be sure that we have the quickest implementation in bothc case
 */
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <signal.h>
#include "stats.h"
#include <asm/signal.h>

#define MALLOC_PAGESIZE_MIN 4096
#define MALLOC_INCR_PAGES 8192

// usable bins from stacbuck...nbuck - 1
#undef HAVE_MMAP
#undef MALLOC_STATS
#undef MALLOC_REGISTER
#undef MALLOC_WATCH

#undef HAVE_GETPAGESIZE
#define NBUCKETS 28

typedef void *t_addr;

/**
 * implementation of magic bytes that check
 * the correctheness of the datas
 */
#define ISALLOC 0xf7
#define ISFREE 0x54
/* Stored before the value returned by
memalign, with the rest of the word
being the distance to the true
beginning of the block.  */
#define ISMEMALILGN 0Xd6
#define MAGIC8_NUMBYTES 8
#define MALLOC_SIZE_T u_bits32_t
#define MALIGN_MASK 15
#define MAGIC1 0x55
#define MAGIC2 0x5555
#define MSLOP 4
#define SPLIT_MIN 1
#define SPLIT_MID 9
#define SPLIT_MAX 12
#define COMBINE_MIN 1
#define LESSCORE_MIN 8
#define LESSCORE_FRC 11
#define PREPOP_BIN 1
#define PREPOP_SIZE 64
#define STARTBUCK 0
#define NBUCKETS 28

#ifdef HAVE_MMAP
#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

#if defined(HAVE_MMAP) && defined(MAP_ANONYMOUS)
#define USE_MMAP 1
#else
#define USE_MMAP 0
#endif

#if USE_MMAP
#define MMAP_THRESHOLD 12 // Must be >= SPLIT_MAX, COMBINE_MAX
#else
#define MMAP_THRESHOLD 64
#endif

#if (USE_MMAP == 1) && defined(HAVE_MREMAP) && defined(MREMAP_MAYMOVE)
#define USE_MREMAP 1
#else
#undef USE_MREMAP
#define USE_MREMAP 0
#endif

#ifdef MALLOC_STATS
struct _malstats _mstats;
#endif /* MALLOC_STATS */

typedef enum e_stmalloc
{
	MALLOC_WRAPPER = 0X01,
	MALLOC_INTERNAL = 0X02,
	MALLOC_NOTRACE = 0X04,
	MALLOC_NOREG = 0X08,
} t_stmalloc;

typedef enum e_err
{
	ERR_DUPFREE = 0x01,
	ERR_UNALLOC = 0x02,
	ERR_UNDERFLOW = 0x04,
	ERR_ASSERT_FAILED = 0x08,
} t_err;

/**
 * Guard bytes we write at the end of the allocation, encoding the size
 */
typedef union u_malloc_guard
{
	char s[4];
	uint32_t i;
} t_mguard;

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
	uint64_t mh_align[2]; // 16
	struct
	{
		uint8_t mi_alloc;	  // ISALLOC OR ISFREE states
		uint8_t mi_index;	  // index in nextf[]
		uint16_t mi_magic2;	  // should be MAGIC2 // *2
		uint32_t mi_nbytes;	  // # of bytes allocated // *4
		uint8_t mi_magic8[8]; // MAGIC1 guard bytes // 8
	} s_minfo;
} t_mhead;

typedef union u_alloc_guard
{
	uint8_t s[4];
	uint32_t i;
} t_mguard;

typedef struct s_glob
{
	int pagesz;
	int pagebucket;
	int maxbuck;
	char *memtop;
	char busy[NBUCKETS];
	t_mhead *nextf[NBUCKETS];
	uint64_t binsizes[NBUCKETS];
	// future feature
	int malloc_flags;
	int malloc_trace;
	int malloc_register;
	int malloc_mmap_threshold;
	int errn;
} t_glob;

#ifdef DEBUG
typedef struct s_glob
{
	int pagesz;
	int pagebucket;
	int maxbuck;
	char *memtop;
	char busy[NBUCKETS];
	t_mhead *nextf[NBUCKETS];
	uint64_t binsizes[NBUCKETS];
	int malloc_flags;
	int malloc_trace;
	int malloc_register;
	int malloc_mmap_threshold;
	int errn;
} t_glob;
#endif

#ifdef MALLOC_TRACE
typedef struct s_glob
{
	int pagesz;
	int pagebucket;
	int maxbuck;
	char *memtop;
	char busy[NBUCKETS];
	t_mhead *nextf[NBUCKETS];
	uint64_t binsizes[NBUCKETS];
	int malloc_flags;
	int malloc_trace;
	int malloc_register;
	int malloc_mmap_threshold;
	int errn;
	char malloc_trace_buckets[NBUCKETS];
} t_glob;

// void mtrace_alloc(const char *, PTR_T, size_t, const char *, int);
// void mtrace_free(PTR_T, int, const char *, int);
#endif

#ifdef MALLOC_STATS
typedef struct s_glob
{
	int pagesz;
	int pagebucket;
	int maxbuck;
	char *memtop;
	char busy[NBUCKETS];
	t_mhead *nextf[NBUCKETS];
	uint64_t binsizes[NBUCKETS];
	int malloc_flags;
	int malloc_trace;
	int malloc_register;
	int malloc_mmap_threshold;
	int errn;
	char malloc_trace_buckets[NBUCKETS];
	t_stats mstats;
} t_glob;

#endif
static inline int mover_head(void)
{
	return (sizeof(t_mhead));
}

typedef struct s_binspec
{
	const size_t *bins;
	size_t len;
} t_binspec;

typedef enum e_glob_field
{
	GLOB_NONE = -1,
	GLOB_PAGESZ,
	GLOB_PAGEBUCKET,
	GLOB_MAXBUCK,
	GLOB_MEMTOP,
	GLOB_BINSIZE,
	GLOB_ERRN
} t_glob_field;

t_addr malloc(size_t size);
t_addr realloc(t_addr mem, size_t nbytes);
void free(t_addr mem);
t_addr ft_memalign(size_t align, size_t size);
t_addr valloc(size_t size);
t_addr ft_calloc(size_t n, size_t s);
void ft_cfree(t_addr mem);
size_t allocated_bytes(size_t n);
t_mhead *chain(t_mhead *a);
t_mhead **chain_ptr(t_mhead *a);
void malloc_stats_sbrk_update(long sbrk_needed);
int pagealign(void);
void register_malloc(t_addr mem, int e,
					 const char *s, const char *file, int line);
void botch(const char *s, const char *file,
		   int line);
void xbotch(t_addr mem, int e, const char *s,
			const char *file, int line);
void bcoalesce(int nu);

/**
 * HELPERS
 */
t_glob *get_glob(t_glob_field field, void *value);
void bcoalesce(int nu);

#ifdef MALLOC_REGISTER

void register_malloc_impl(t_addr mem, int e, const char *s, int line);
#else

void register_malloc_impl(t_addr mem, int e, const char *s,
						  const char *file, int line);
#endif

void botch(const char *s, const char *file, int line);
void xbotch(t_addr mem, int e, const char *s, const char *file, int line);
void bsplit(int nu);
void xsplit(t_mhead *mp, int nu);
bool in_bucket(size_t nb, int nu);
bool right_bucket(size_t nb, int nu);
size_t allocated_bytes(size_t n);
void compute_stats_core(t_glob *g, int nu, int flag, size_t value);
void compute_stats_brk(t_glob *g, size_t value);
void compute_stats_mmap(t_glob *g, size_t value);
int is_powerof2(int x);
void assert_or_abort(int cond, const char *expr, const char *file, int line);
#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   private.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:51:34 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:34:43 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRIVATE_H
#define PRIVATE_H

#include "alloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

typedef struct s_ifree_ctx
{
    t_addr mem;
    t_mhead *p;
    char *ap;
    int nunits;
    uint32_t nbytes;
    uint32_t ubytes;
    t_glob *g;
    int freed;
} t_ifree_ctx;

void validate_end_guard(char *ap, t_mhead *p, t_addr mem, const char *file, int line);
int handle_special_free_cases(t_mhead *p, int nunits, t_glob *g);
int try_munmap_free(t_mhead *p, int nunits, t_glob *g);
int try_lesscore_free(t_mhead *p, int nunits, t_glob *g);
void add_to_free_list(t_mhead *p, int nunits, t_glob *g);
void handle_busy_bucket(t_mhead *p, int nunits, t_glob *g);
void trace_and_watch_setup(t_mhead *p, uint32_t *ubytes);
void handle_memalign(t_addr *mem, char **ap, t_mhead **p);
void validate_alloc_status(t_addr mem, t_mhead *p, const char *file, int line);
void validate_magic8(t_mhead *p, t_addr mem, const char *file, int line);
void ifree_prepare(t_ifree_ctx *c, const char *file, int line);
void ifree_handle_free(t_ifree_ctx *c);
int calculate_nunits(uint32_t nbytes, t_glob *g);
int handle_reentrant(int nunits, t_glob *g);
t_mhead *get_block_from_freelist(int nunits, t_glob *g);
void validate_free_block(t_mhead *p, int nunits, const char *file, int line);
void setup_block_header(t_mhead *p, size_t n);
void setup_end_guard(t_mhead *p, size_t n);

// Malloc triggers
void scramble_allocated_memory(void *mem, size_t n);
void update_recurse_stats(t_glob *g, int nunits);
void update_alloc_stats(t_glob *g, int nunits, size_t n);
void handle_malloc_trace_alloc(t_addr ptr, size_t n, int nunits, const char *file, int line, int flags, t_glob *g);
void handle_malloc_register_alloc(t_addr ptr, size_t n, const char *file, int line, int flags, t_glob *g);
void handle_malloc_watch_alloc(t_addr ptr, size_t n, const char *file, int line, t_glob *g);
void check_alignment(t_addr ptr, t_mhead *p, const char *file, int line);
int check_max_alloc_size(size_t n);

/* helper used by free path to decide whether to sample trace/register/watch */
int should_trace_or_register_or_watch(void);

void set_state_mem(t_addr mem);
t_addr get_state_mem(void);
void ensure_init(void);

/* Parameters for mremap_implement: wrap many args into one pointer to satisfy
 * style constraints (single argument) and make refactoring easier.
 */
typedef struct s_mremap_params
{
    t_addr mem;
    size_t n;
    int newunits;
    uint32_t tocopy;
    int flags;
    const char *file;
    int line;
    t_glob *g;
    int nunits;
} t_mremap_params;

/* New prototype: single-argument entry point */
t_addr mremap_implement(t_mremap_params *params);

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 02:57:25 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:16:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
#define GET_NEXT_LINE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "alloc.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#define FD_MAX 10000
#define DFLT_CAP 64

/* return states used by the get_next_line helpers */
typedef enum e_state
{
	ST_ERR_ALLOC = -2,		/* allocation failed */
	ST_FILE_NOT_FOUND = -1, /* read returned -1 */
	ST_EOF = 0,				/* read returned 0 (EOF) */
	ST_OK = 1,				/* generic OK / boolean true */
	ST_FILLED = 2,			/* buffer was refilled with data */
	ST_FOUND_NL = 3,		/* newline found */
	ST_SCANNING = 1			/* used as while(ST_SCANNING) loop flag */
} t_state;

/* bounded strchr: find first occurrence of c in s within n bytes */
static inline char *strnchr(const char *s, int c, size_t n)
{
	size_t i;

	if (!s)
		return (NULL);
	i = 0;
	while (i < n)
	{
		if (s[i] == (char)c)
			return ((char *)(s + i));
		i++;
	}
	return (NULL);
}

typedef struct s_file
{
	char buf[BUFFER_SIZE];
	char *cur;
	char *end;
} t_file;

typedef struct s_dynstr
{
	char *buf;
	size_t size;
	size_t cap;
} t_dynstr;

/* prototypes now see t_state correctly */
char *get_next_line(int fd);
char *get_next_line_bonus(int fd);
t_state append_from_buffer(t_file *scan, t_dynstr *line);
t_state refill(t_file *scan, int fd);
t_state scan_nl(t_file *scan, t_dynstr *line, int fd);

/* Choose allocator at compile time:
 * - If MODE_MALLOC is defined, use libc malloc/realloc/free.
 * - Otherwise use the custom ft_malloc/ft_realloc/ft_free.
 */
#ifdef MODE_MALLOC
#define GNL_MALLOC malloc
#define GNL_REALLOC realloc
#define GNL_FREE free
#else
#define GNL_MALLOC ft_malloc
#define GNL_REALLOC ft_realloc
#define GNL_FREE ft_free
#endif

static inline void init(t_file *scan)
{
	if (!scan)
	{
		scan = GNL_MALLOC(sizeof(t_file));
		if (!scan)
			return;
		scan->cur = scan->buf;
		scan->end = scan->buf;
	}
}

static inline char *reset(t_dynstr *line, t_file *scan)
{
	if (line)
	{
		GNL_FREE(line->buf);
		line->buf = NULL;
		line->size = 0;
		line->cap = 0;
	}
	if (scan)
	{
		scan->cur = scan->buf;
		scan->end = scan->buf;
	}
	return (NULL);
}

static inline t_state ensure_cap(char **line, size_t *cap, size_t need)
{
	void *tmp;
	size_t new_cap;

	if (*cap >= need)
		return (ST_OK);
	if (*cap)
		new_cap = *cap;
	else
		new_cap = DFLT_CAP;
	while (new_cap <= need)
		new_cap *= 2;
	tmp = GNL_REALLOC(*line, new_cap);
	if (!tmp)
		return (ST_ERR_ALLOC);
	*cap = new_cap;
	*line = (char *)tmp;
	return (ST_OK);
}

#endif

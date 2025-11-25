/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 00:55:37 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:49:46 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <string.h>
#include <stdlib.h>

t_state append_from_buffer(t_file *scan, t_dynstr *line)
{
	char *nl;
	size_t chunk;
	size_t avail;

	avail = (size_t)(scan->end - scan->cur);
	nl = (char *)memchr(scan->cur, '\n', avail);
	if (nl)
		chunk = (size_t)(nl - scan->cur + 1);
	else
		chunk = avail;
	if (ensure_cap(&line->buf, &line->cap, line->size + chunk + 1) == ST_ERR_ALLOC)
		return (ST_ERR_ALLOC);
	memmove(line->buf + line->size, scan->cur, chunk);
	line->size += chunk;
	line->buf[line->size] = '\0';
	scan->cur += chunk;
	return (nl != NULL);
}

t_state refill(t_file *scan, int fd)
{
	ssize_t readn;

	readn = read(fd, scan->buf, BUFFER_SIZE);
	if (readn <= 0)
		return ((int)readn);
	scan->cur = scan->buf;
	scan->end = scan->buf + readn;
	return (ST_FILLED);
}

t_state scan_nl(t_file *scan, t_dynstr *line, int fd)
{
	t_state st;

	while (ST_SCANNING)
	{
		if (scan->cur >= scan->end)
		{
			st = refill(scan, fd);
			if (st == ST_FILE_NOT_FOUND)
				return (ST_FILE_NOT_FOUND);
			if (st == 0)
				return (ST_EOF);
			if (st != ST_FILLED)
				return (st);
		}
		st = append_from_buffer(scan, line);
		if (st == ST_ERR_ALLOC)
			return (ST_ERR_ALLOC);
		if (st)
			break;
	}
	return (ST_FOUND_NL);
}

char *get_next_line(int fd)
{
	static t_file scan;
	t_dynstr line;
	t_state st;

	line = (t_dynstr){NULL, 0, 0};
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (scan.cur == NULL || scan.end == NULL)
	{
		scan.cur = scan.buf;
		scan.end = scan.buf;
	}
	st = scan_nl(&scan, &line, fd);
	if (st == ST_ERR_ALLOC || st == ST_FILE_NOT_FOUND)
		return (reset(&line, &scan));
	if (st == ST_EOF)
	{
		if (line.size > 0)
			return (line.buf);
		return (reset(&line, &scan));
	}
	if (line.size == 0)
		return (reset(&line, NULL));
	return (line.buf);
}

char *get_next_line_bonus(int fd)
{
	static t_file *scan[FD_MAX] = {0};
	t_dynstr line;
	t_state st;

	line = (t_dynstr){NULL, 0, 0};
	if (fd < 0 || fd >= FD_MAX || BUFFER_SIZE <= 0)
		return (NULL);
	init(scan[fd]);
	st = scan_nl(scan[fd], &line, fd);
	if (st == ST_ERR_ALLOC || st == ST_FILE_NOT_FOUND)
		return (reset(&line, scan[fd]));
	if (st == ST_EOF)
	{
		if (line.size > 0)
			return (line.buf);
		return (reset(&line, scan[fd]));
	}
	if (line.size == 0)
		return (reset(&line, NULL));
	return (line.buf);
}

/* main: read from file given as argv[1] or from stdin when no arg supplied.
 * Print each line returned by get_next_line and free it.
 */
int main(int argc, char **argv)
{
	int fd;
	char *line;

	(void)argc;
	fd = 0;
	if (argv && argv[1])
	{
		fd = open(argv[1], O_RDONLY);
		if (fd < 0)
		{
			perror("open");
			return (1);
		}
	}
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("%s", line);
		GNL_FREE(line);
	}
	if (argv && argv[1])
		close(fd);
	return (0);
}

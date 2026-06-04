/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   track.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:51:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/05 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"

t_alloc_entry	*alloc_table(void);
int				*alloc_count_ptr(void);
unsigned long	*alloc_overflow_ptr(void);
int				tbl_find(t_alloc_entry *e, void *ptr);
int				tbl_insert_slot(t_alloc_entry *e, void *ptr);
void			tbl_delete_at(t_alloc_entry *e, size_t i);

void	track_allocation_dbg(void *ptr, size_t size, const char *file, int line)
{
	static unsigned long	serial;
	t_alloc_entry			*e;
	int						i;

	if (!ptr)
		return ;
	e = alloc_table();
	i = tbl_insert_slot(e, ptr);
	if (i < 0)
	{
		(*alloc_overflow_ptr())++;
		return ;
	}
	e[i].ptr = ptr;
	e[i].size = size;
	e[i].active = 1;
	e[i].file = file;
	e[i].line = line;
	e[i].serial = serial++;
	(*alloc_count_ptr())++;
}

void	track_allocation(void *ptr, size_t size)
{
	track_allocation_dbg(ptr, size, (const char *)0, 0);
}

void	untrack_allocation(void *ptr)
{
	t_alloc_entry	*e;
	int				i;

	if (!ptr)
		return ;
	e = alloc_table();
	i = tbl_find(e, ptr);
	if (i < 0)
		return ;
	tbl_delete_at(e, (size_t)i);
	(*alloc_count_ptr())--;
}

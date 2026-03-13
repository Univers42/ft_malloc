/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 14:50:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 21:24:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

# include "alloc.h"
# include <stdio.h>
# include <stdlib.h>

# define TINY_MAX 128
# define SMALL_MAX 1024
# define MAX_TRACKED_ALLOCS 10000

typedef struct s_alloc_entry
{
	void	*ptr;
	size_t	size;
	int		active;
}	t_alloc_entry;

#endif

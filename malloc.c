/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 18:56:10 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/31 19:10:43 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

t_addr  internal_malloc(size_t n, const char *file, int line, int flags)
{
	t_mhead *p;
	int		nunits;
	char	*m;
	char	*z;
	uint32_t	nbytes;
	t_mguard	mg;

	if (pagesz == 0)
		if (page)
}
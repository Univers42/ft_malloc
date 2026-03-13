/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   valloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:44:26 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 23:47:35 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "private/private.h"

#ifndef NO_VALLOC

t_addr	ft_valloc(size_t size)
{
	t_val_ctx	v;

	ensure_init();
	v.mem = NULL;
	v.file = NULL;
	v.line = 0;
	return (internal_valloc(size, &v, 0));
}

#endif

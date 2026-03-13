/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_valloc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:52:02 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 22:54:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

t_addr	internal_valloc(size_t size, t_val_ctx *v, int flags)
{
	return (internal_memalign(getpagesize(), size,
			v, flags | MALLOC_INTERNAL));
}

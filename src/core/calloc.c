/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:47:30 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:35:46 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifndef NO_CALLOC

t_addr	ft_calloc(size_t n, size_t s)
{
	ensure_init();
	return (internal_calloc(n, s, (char *) NULL, 0, 0));
}

void	ft_cfree(t_addr mem)
{
	ensure_init();
	internal_cfree(mem, (char *) NULL, 0, 0);
}

#endif
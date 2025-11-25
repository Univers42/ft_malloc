/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   valloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:44:26 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:38:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifndef NO_VALLOC

t_addr	ft_valloc(size_t size)
{
	ensure_init();
	return (internal_valloc(size, (char *) NULL, 0, 0));
}

#endif

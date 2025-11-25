/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 18:56:10 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:37:52 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

t_addr	ft_malloc(size_t size)
{
	ensure_init();
	return (internal_malloc(size, (char *) NULL, 0, 0));
}

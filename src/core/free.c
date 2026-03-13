/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:06:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 23:47:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "private/private.h"

void	ft_free(t_addr mem)
{
	ensure_init();
	internal_free(mem, (char *) NULL, 0, 0);
}

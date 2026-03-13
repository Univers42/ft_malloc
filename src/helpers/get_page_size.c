/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_page_size.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:05:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:05:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_page_size.h"

int	get_page_size(void)
{
	long	result;

	result = sysconf(_SC_PAGESIZE);
	if (result > 0)
		return ((int)result);
	return (4096);
}

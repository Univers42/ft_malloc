/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memset.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 20:00:27 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:02:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <string.h>
#include <stdint.h>

/* small helper: fill up to 32 bytes efficiently */
static void	small_memset(uint8_t *dst, int xch, size_t nbytes)
{
	size_t	remaining;

	remaining = nbytes;
	while (remaining >= 8)
	{
		*dst++ = (uint8_t)xch;
		*dst++ = (uint8_t)xch;
		*dst++ = (uint8_t)xch;
		*dst++ = (uint8_t)xch;
		*dst++ = (uint8_t)xch;
		*dst++ = (uint8_t)xch;
		*dst++ = (uint8_t)xch;
		*dst++ = (uint8_t)xch;
		remaining -= 8;
	}
	while (remaining > 0)
	{
		*dst++ = (uint8_t)xch;
		remaining--;
	}
}

/* public API: small-path optimized, large-path delegates to libc memset */
void	malloc_memset(void *charp, int xch, size_t nbytes)
{
	if (nbytes == 0)
		return ;
	if (nbytes <= 32)
		small_memset((uint8_t *)charp, xch, nbytes);
	else
		memset(charp, xch, nbytes);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zero.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 20:03:12 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:06:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <string.h>

void	malloc_zero(void *charp, size_t nbytes)
{
	size_t	mzsz;
	size_t	*mz;
	size_t	words;
	size_t	rem;

	mzsz = nbytes;
	if (mzsz <= (9 << 3))
	{
		mz = (size_t *)charp;
		words = mzsz >> 3;
		while (words--)
			*mz++ = 0;
		rem = mzsz & 7;
		if (rem)
			memset(mz, 0, rem);
	}
	else
		memset(charp, 0, mzsz);
}

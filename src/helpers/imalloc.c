/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imalloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 13:05:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:26:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

void malloc_memset(void *charp, int xch, size_t nbytes)
{
	uint8_t *mzp;
	size_t remaining;

	if (nbytes == 0)
		return;
	if (nbytes <= 32)
	{
		mzp = (uint8_t *)charp;
		remaining = nbytes;
		while (remaining >= 8)
		{
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			remaining -= 8;
		}
		while (remaining > 0)
		{
			*mzp++ = (uint8_t)xch;
			remaining--;
		}
	}
	else
		memset(charp, xch, nbytes);
}

void malloc_bzero(void *charp, size_t nbytes)
{
	size_t *mzp;
	size_t chunks;
	size_t i;

	if (nbytes == 0)
		return;
	if (nbytes <= 32)
	{
		mzp = (size_t *)charp;
		chunks = nbytes >> 3;
		i = 0;
		while (i < chunks)
		{
			*mzp++ = 0;
			i++;
		}
	}
	else
		memset(charp, 0, nbytes);
}

void malloc_zero(void *charp, size_t nbytes)
{
	size_t mzsz;
	size_t *mz;

	mzsz = nbytes;
	if (mzsz <= (9 << 3))
	{
		mz = (size_t *)charp;
		if (mzsz >= (5 << 3))
		{
			*mz++ = 0;
			*mz++ = 0;
			if (mzsz >= (7 << 3))
			{
				*mz++ = 0;
				*mz++ = 0;
				if (mzsz >= (9 << 3))
				{
					*mz++ = 0;
					*mz++ = 0;
				}
			}
		}
		*mz++ = 0;
		*mz++ = 0;
		*mz = 0;
	}
	else
		memset(charp, 0, mzsz);
}

void malloc_memcpy(void *dest, const void *src, size_t nbytes)
{
	size_t *mcsrc;
	size_t *mcdst;
	size_t chunks;
	size_t i;
	uint8_t *bsrc;
	uint8_t *bdst;
	size_t remainder;

	if (nbytes == 0)
		return;
	if (nbytes <= 32)
	{
		mcsrc = (size_t *)src;
		mcdst = (size_t *)dest;
		chunks = nbytes >> 3;
		i = 0;
		while (i < chunks)
		{
			*mcdst++ = *mcsrc++;
			i++;
		}
		remainder = nbytes & 7;
		if (remainder)
		{
			bsrc = (uint8_t *)mcsrc;
			bdst = (uint8_t *)mcdst;
			i = 0;
			while (i < remainder)
			{
				*bdst++ = *bsrc++;
				i++;
			}
		}
	}
	else
		memcpy(dest, src, nbytes);
}

#if __GNUC__ > 1
void fastcopy(const void *s, void *d, size_t n)
{
	__builtin_memcpy(d, s, n);
}
#else
#if !defined(HAVE_BCOPY)
#if !defined(HAVE_MEMMOVE)
void fastcopy(const void *s, void *d, size_t n)
{
	memcpy(d, s, n);
}
#else
void fastcopy(const void *s, void *d, size_t n)
{
	memmove(d, s, n);
}
#endif /* !HAVE_MEMMOVE */
#else  /* HAVE_BCOPY */
void fastcopy(const void *s, void *d, size_t n)
{
	bcopy(s, d, n);
}
#endif /* HAVE_BCOPY */
#endif /* !__GNUC__ */

size_t malloc_usable_size(void *mem)
{
	t_mhead *p;
	char *ap;

	if ((ap = (char *)mem) == 0)
		return (0);
	p = (t_mhead *)ap - 1;
	if (p->s_minfo.mi_alloc == ISMEMALILGN)
	{
		ap -= p->s_minfo.mi_nbytes;
		p = (t_mhead *)ap - 1;
	}
	if (p->s_minfo.mi_alloc == ISFREE)
		return (0);
	return (p->s_minfo.mi_nbytes);
}
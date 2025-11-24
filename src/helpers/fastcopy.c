/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fastcopy.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 19:53:49 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 19:58:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#if __GNUC__ > 1

void	fastcopy(const void *s, void *d, size_t n)
{
	__builtin_memcpy(d, s, n);
}
#else

# if !defined(HAVE_BCOPY)

#  if !defined(HAVE_MEMMOVE)

void	fastcopy(const void *s, void *d, size_t n)
{
	memcpy(d, s, n);
}
#  else

void	fastcopy(const void *s, void *d, size_t n)
{
	memmove(d, s, n);
}
#  endif /* !HAVE_MEMMOVE */

# else  /* HAVE_BCOPY */

void	fastcopy(const void *s, void *d, size_t n)
{
	bcopy(s, d, n);
}
# endif /* HAVE_BCOPY */

#endif /* !__GNUC__ */

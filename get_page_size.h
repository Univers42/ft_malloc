/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_page_size.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 19:14:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/31 19:24:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_PAGE_SIZE_H
# define GET_PAGE_SIZE_H

# include <unistd.h>
# include <sys/types.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>

/* Detect which sysconf selector is available */
# if defined(_SC_PAGESIZE)
#  define PAGE_SYSCONF _SC_PAGESIZE
# elif defined(_SC_PAGE_SIZE)
#  define PAGE_SYSCONF _SC_PAGE_SIZE
# else
#  define PAGE_SYSCONF 0
# endif

/* Fall back to compile-time constants when sysconf selectors are not present */
# if PAGE_SYSCONF == 0
#  if defined(PAGESIZE)
#   define PAGE_SIZE_CONST PAGESIZE
#  elif defined(EXEC_PAGESIZE)
#   define PAGE_SIZE_CONST EXEC_PAGESIZE
#  else
#   define PAGE_SIZE_CONST 4096
#  endif
# endif

static inline int	get_page_size(void)
{
	if (PAGE_SYSCONF != 0)
		return ((int) sysconf(PAGE_SYSCONF));
	else
		return (PAGE_SIZE_CONST);
}

#endif /* GET_PAGE_SIZE_H */

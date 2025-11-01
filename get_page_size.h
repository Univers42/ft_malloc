/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_page_size.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 19:14:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 19:03:03 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_PAGE_SIZE_H
# define GET_PAGE_SIZE_H

# include <unistd.h>
# include <sys/types.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>

// #if defined (HAVE_UNISTD_H)
// #  ifdef _MINIX
// #    include <sys/types.h>
// #  endif
// #  include <unistd.h>
// #  if defined (_SC_PAGESIZE)
// #    define getpagesize() sysconf(_SC_PAGESIZE)
// #  else
// #    if defined (_SC_PAGE_SIZE)
// #      define getpagesize() sysconf(_SC_PAGE_SIZE)
// #    endif /* _SC_PAGE_SIZE */
// #  endif /* _SC_PAGESIZE */
// #endif

// #if !defined (getpagesize)
// #  if defined (HAVE_SYS_PARAM_H)
// #    include <sys/param.h>
// #  endif
// #  if defined (PAGESIZE)
// #     define getpagesize() PAGESIZE
// #  else /* !PAGESIZE */
// #    if defined (EXEC_PAGESIZE)
// #      define getpagesize() EXEC_PAGESIZE
// #    else /* !EXEC_PAGESIZE */
// #      if defined (NBPG)
// #        if !defined (CLSIZE)
// #          define CLSIZE 1
// #        endif /* !CLSIZE */
// #        define getpagesize() (NBPG * CLSIZE)
// #      else /* !NBPG */
// #        if defined (NBPC)
// #          define getpagesize() NBPC
// #        endif /* NBPC */
// #      endif /* !NBPG */
// #    endif /* !EXEC_PAGESIZE */
// #  endif /* !PAGESIZE */
// #endif /* !getpagesize */

// #if !defined (getpagesize)
// #  define getpagesize() 4096  /* Just punt and use reasonable value */
// #endif


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

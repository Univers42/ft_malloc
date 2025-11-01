/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:23:40 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 20:32:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Minimal config.h for ft_malloc - edit for your target platform */
#ifndef CONFIG_H
# define CONFIG_H

/* Platform feature selection ------------------------------------------------*/

/* Enable mmap-backed allocations (set to 0 to force sbrk-only behaviour) */
# define HAVE_MMAP 1

/* mremap is not used by default; enable only if your platform supports it */
# undef HAVE_MREMAP

/* getpagesize() availability (set to 0 if not present) */
# define HAVE_GETPAGESIZE 1

/* Sizes (adjust for 32-bit targets) -----------------------------------------*/
# ifndef SIZEOF_LONG
#  define SIZEOF_LONG 8
# endif

# ifndef SIZEOF_CHAR_P
#  define SIZEOF_CHAR_P 8
# endif

# ifndef SIZEOF_INT
#  define SIZEOF_INT 4
# endif

# ifndef SIZEOF_SIZE_T
#  define SIZEOF_SIZE_T 8
# endif

/* Alignment used by allocator (must be power of two) */
# ifndef MALLOC_ALIGNMENT
#  define MALLOC_ALIGNMENT 16
# endif

/* End ----------------------------------------------------------------------*/
#endif /* CONFIG_H */
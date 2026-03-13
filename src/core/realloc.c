/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:37:56 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 23:33:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "private/private.h"

/*
 * This translation unit intentionally does not define internal_realloc.
 * The full implementation lives in src/core/private/internal_realloc.c
 * to avoid multiple-definition link errors.
 *
 * Public wrapper ft_realloc is provided in suite_alloc.c (or calls
 * internal_realloc directly), so no symbols are needed here.
 */

/* public wrapper: forward to the real implementation in the private TU */
t_addr	ft_realloc(t_addr mem, size_t nbytes)
{
	t_val_ctx	v;

	ensure_init();
	v.mem = 0;
	v.file = NULL;
	v.line = 0;
	return (internal_realloc(mem, nbytes, &v, 0));
}

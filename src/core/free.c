/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:06:39 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:15:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "imalloc.h"

t_addr internal_free(t_addr mem, const char *file, int line, int flags)
{
	t_ifree_ctx ctx;

	if (mem == 0)
		return (NULL);
	ctx.mem = mem;
	ifree_prepare(&ctx, file, line);
	ifree_handle_free(&ctx);
	finalize_free(ctx.mem, ctx.ubytes, ctx.nunits, file, line, flags, ctx.g);
	return (NULL);
}

void ft_free(t_addr mem)
{
	ensure_init();
	internal_free(mem, (char *)NULL, 0, 0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   singletons.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 15:27:03 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:17:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/**
 * xbotch is an extended version assertion handler. it prints a more detailed
 * message that includes file and line, optionally
 * describe the offending allocation
 * (via the MALLOC_REGISTER helper),
 * flushes stderr, then delegates to botch
 * which terminate. So we use xbotch
 * when we want extra context about a memory error
 * botch does the actual shutdown/reporting
 */
t_addr	state_mem_singleton(t_addr mem, int set)
{
	static t_addr	last_mem = NULL;

	if (set)
		last_mem = mem;
	return (last_mem);
}

void	set_state_mem(t_addr mem)
{
	state_mem_singleton(mem, 1);
}

t_addr	get_state_mem(void)
{
	return (state_mem_singleton(NULL, 0));
}

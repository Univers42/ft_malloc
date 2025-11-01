/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chain.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:59:41 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 21:00:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/**
 * read teh pointer stored at ((char *)a + sizeof(char))
 */
t_mhead	*chain(t_mhead *a)
{
	return (*(t_mhead **)((char *)a + sizeof(char *)));
}

/**
 * if we need to reassing thruoght the same location (i.e)
 * we would need to use a lvalue, so I use chain_ptr(a) to obtain
 * t_mhead** and write through it:
 * `*chain_ptr(a) = some_ptr;`
 */
t_mhead	**chain_ptr(t_mhead *a)
{
	return ((t_mhead **)((char *)a + sizeof(char *)));
}

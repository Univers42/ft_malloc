/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imalloc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:13:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:14:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMALLOC_H
# define IMALLOC_H

#include "alloc.h"

typedef struct s_ifree_ctx
{
	t_addr mem;
	t_mhead *p;
	char *ap;
	int nunits;
	uint32_t nbytes;
	uint32_t ubytes;
	t_glob *g;
	int freed;
} t_ifree_ctx;


#endif
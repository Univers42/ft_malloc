/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leakcheck_dtor.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Auto leak report at program exit. Compiled into the library only when
 * FT_MALLOC_DEBUG is defined (i.e. `make debug`); the shipped subject build
 * has no destructor and stays silent on exit.
 */

#include "debug.h"

#ifdef FT_MALLOC_DEBUG

void	leak_report_on_exit(void);

__attribute__((destructor))
static void	ft_malloc_leak_dtor(void)
{
	leak_report_on_exit();
}

#endif

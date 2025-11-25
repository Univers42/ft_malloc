/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   public.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:32:11 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:16:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUBLIC_H
# define PUBLIC_H
#include "alloc.h"

/**
 * Public api of our system
 * This is just nomenclature to guide user where they should look into 
 * the repo.
 */

t_addr  ft_calloc(size_t n, size_t s);
void    ft_cfree(t_addr mem);
void    ft_free(t_addr mem);
t_addr  internal_free(t_addr mem, const char *file, int line, int flags);
t_addr  ft_malloc(size_t size);
t_addr  ft_realloc(t_addr mem, size_t nbytes);
t_addr  ft_valloc(size_t size);

#endif
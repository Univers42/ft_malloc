/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sbrk.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 19:17:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 21:31:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SBRK_H
# define SBRK_H

# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>
# include <sys/syscall.h>

void	sbrk_get_state(void **initialbrk, void **curbrk);

#endif
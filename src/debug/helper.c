/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:05:42 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:37:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

void malloc_debug_dummy(void)
{
    ssize_t ret;

    ret = write(1, "malloc_debug_dummy\n", 19);
    (void)ret;
}

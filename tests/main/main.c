/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 13:50:55 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 13:55:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#include "stdio.h"
#include "string.h"

int main(void)
{
	char    *p;
	char    *q;
	int		*arr;

	ft_malloc(64);
	if (!p)
	{
		perror("ft_malloc");
		return (1);
	}
	strcpy(p, "hello from ft_malloc");
	puts(p);
	q = ft_realloc(p, 128);
	if (!q)
	{
		perror("ft_realloc");
		ft_free(p);
		return(1);
	}
	strcat(q, "and ft_realloc");
	puts(q);

	arr = ft_calloc(10, sizeof(*arr));
	if (!arr)
	{
		perror("ft_calloc");
		ft_free(q);
		return (1);
	}
	arr[0] = 42;
	printf("arr[0] = %d\n", arr[0]);
	ft_free(arr);
	ft_free(q);
	return (0);
}
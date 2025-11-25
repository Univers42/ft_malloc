/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 13:50:55 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:27:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "stdio.h"
#include "string.h"
#include <time.h>

static void test_basic_alloc(void)
{
	char *p;

	printf("\n=== Test 1: Basic allocation ===\n");
	p = ft_malloc(64);
	if (!p)
	{
		fprintf(stderr, "ft_malloc(64) failed\n");
		return;
	}
	strcpy(p, "Hello from ft_malloc!");
	printf("Allocated 64 bytes: %s\n", p);
	show_alloc_mem(); // <-- Show after allocation
	ft_free(p);
	printf("Freed successfully\n");
}

static void test_realloc(void)
{
	char *p;
	char *q;

	printf("\n=== Test 2: Realloc ===\n");
	p = ft_malloc(32);
	if (!p)
	{
		fprintf(stderr, "ft_malloc(32) failed\n");
		return;
	}
	strcpy(p, "Small");
	printf("Original (32 bytes): %s\n", p);

	q = ft_realloc(p, 128);
	if (!q)
	{
		fprintf(stderr, "ft_realloc(128) failed\n");
		ft_free(p);
		return;
	}
	strcat(q, " -> Bigger");
	printf("After realloc (128 bytes): %s\n", q);
	show_alloc_mem(); // <-- Show after realloc
	ft_free(q);
	printf("Freed successfully\n");
}

static void test_calloc(void)
{
	int *arr;
	int i;
	int all_zero;

	printf("\n=== Test 3: Calloc (zeroed memory) ===\n");
	arr = ft_calloc(20, sizeof(int));
	if (!arr)
	{
		fprintf(stderr, "ft_calloc failed\n");
		return;
	}

	all_zero = 1;
	i = 0;
	while (i < 20)
	{
		if (arr[i] != 0)
		{
			all_zero = 0;
			break;
		}
		i++;
	}
	printf("Allocated 20 ints, all zero: %s\n", all_zero ? "YES" : "NO");

	i = 0;
	while (i < 20)
	{
		arr[i] = i * 10;
		i++;
	}
	printf("Set values: arr[0]=%d, arr[10]=%d, arr[19]=%d\n", arr[0], arr[10], arr[19]);
	show_alloc_mem(); // <-- Show after calloc
	ft_free(arr);
	printf("Freed successfully\n");
}

static void test_multiple_sizes(void)
{
	void *ptrs[10];
	size_t sizes[10] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
	int i;

	printf("\n=== Test 4: Multiple allocation sizes ===\n");
	i = 0;
	while (i < 10)
	{
		ptrs[i] = ft_malloc(sizes[i]);
		if (!ptrs[i])
		{
			fprintf(stderr, "ft_malloc(%zu) failed\n", sizes[i]);
			while (--i >= 0)
			{
				ft_free(ptrs[i]);
				return;
			}
		}
		memset(ptrs[i], 'A' + i, sizes[i]);
		i++;
	}
	printf("Allocated 10 blocks of varying sizes\n");
	show_alloc_mem(); // <-- Show after allocations

	i = 0;
	while (i < 10)
	{
		ft_free(ptrs[i]);
		i++;
	}
	printf("Freed all blocks successfully\n");
}

static void test_stress(void)
{
	void *ptrs[100];
	int i;
	int alloc_count;
	int free_count;

	printf("\n=== Test 5: Stress test (100 allocations) ===\n");
	alloc_count = 0;
	i = 0;
	while (i < 100)
	{
		size_t size = ((i * 17) % 500) + 16;
		ptrs[i] = ft_malloc(size);
		if (ptrs[i])
		{
			memset(ptrs[i], (i % 26) + 'a', size);
			alloc_count++;
		}
		i++;
	}
	printf("Successfully allocated %d/100 blocks\n", alloc_count);
	show_alloc_mem(); // <-- Show after stress allocations

	free_count = 0;
	i = 0;
	while (i < 100)
	{
		if (ptrs[i])
		{
			ft_free(ptrs[i]);
			free_count++;
		}
		i++;
	}
	printf("Successfully freed %d blocks\n", free_count);
}

static void test_large_alloc(void)
{
	void *large;
	size_t size;

	printf("\n=== Test 6: Large allocation ===\n");
	size = 1024 * 1024;
	large = ft_malloc(size);
	if (!large)
	{
		fprintf(stderr, "ft_malloc(%zu) failed\n", size);
		return;
	}
	printf("Allocated 1MB successfully\n");
	memset(large, 'X', size);
	printf("Filled with data successfully\n");
	ft_free(large);
	printf("Freed successfully\n");
}

static void test_realloc_patterns(void)
{
	char *p;
	int i;

	printf("\n=== Test 7: Realloc chain ===\n");
	p = ft_malloc(16);
	if (!p)
	{
		fprintf(stderr, "Initial malloc failed\n");
		return;
	}
	strcpy(p, "Start");
	printf("Initial: %s (16 bytes)\n", p);

	i = 0;
	while (i < 5)
	{
		size_t new_size = 16 * (2 << i);
		p = ft_realloc(p, new_size);
		if (!p)
		{
			fprintf(stderr, "realloc to %zu failed\n", new_size);
			return;
		}
		printf("Realloc %d: %zu bytes\n", i + 1, new_size);
		i++;
	}
	ft_free(p);
	printf("Freed successfully\n");
}

__attribute__((weak))
int main(void)
{
	printf("╔═══════════════════════════════════════╗\n");
	printf("║   FT_MALLOC Comprehensive Test Suite  ║\n");
	printf("╚═══════════════════════════════════════╝\n");

	test_basic_alloc();
	test_realloc();
	test_calloc();
	test_multiple_sizes();
	test_stress();
	test_large_alloc();
	test_realloc_patterns();

	printf("\n╔═══════════════════════════════════════╗\n");
	printf("║   Final Memory State (Leak Check)     ║\n");
	printf("╚═══════════════════════════════════════╝\n");
	show_alloc_mem(); // ← Shows leaks if any remain

	printf("\n╔═══════════════════════════════════════╗\n");
	printf("║   All tests completed successfully!   ║\n");
	printf("╚═══════════════════════════════════════╝\n");
	return (0);
}

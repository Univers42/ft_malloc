/* ~50 benchmark scenarios: {name, kind, a, b, n, iters} */
#ifndef BENCH_SCN_H
# define BENCH_SCN_H

static const t_scn	g_scn[] = {
/* malloc+touch+free latency across size classes */
{"churn 8B", K_CHURN, 8, 0, 0, 300000},
{"churn 16B", K_CHURN, 16, 0, 0, 300000},
{"churn 24B", K_CHURN, 24, 0, 0, 300000},
{"churn 32B", K_CHURN, 32, 0, 0, 300000},
{"churn 48B", K_CHURN, 48, 0, 0, 300000},
{"churn 64B", K_CHURN, 64, 0, 0, 300000},
{"churn 96B", K_CHURN, 96, 0, 0, 300000},
{"churn 128B", K_CHURN, 128, 0, 0, 300000},
{"churn 192B", K_CHURN, 192, 0, 0, 250000},
{"churn 256B", K_CHURN, 256, 0, 0, 250000},
{"churn 384B", K_CHURN, 384, 0, 0, 200000},
{"churn 512B", K_CHURN, 512, 0, 0, 200000},
{"churn 768B", K_CHURN, 768, 0, 0, 200000},
{"churn 1024B", K_CHURN, 1024, 0, 0, 200000},
{"churn 2048B", K_CHURN, 2048, 0, 0, 150000},
{"churn 4096B", K_CHURN, 4096, 0, 0, 100000},
{"churn 16KB", K_CHURN, 16384, 0, 0, 60000},
{"churn 64KB", K_CHURN, 65536, 0, 0, 30000},
{"churn 256KB", K_CHURN, 262144, 0, 0, 15000},
{"churn 1MB", K_CHURN, 1048576, 0, 0, 4000},
/* allocate N live then free (LIFO / FIFO order) */
{"batchLIFO 16B x512", K_BATCH_LIFO, 16, 0, 512, 300},
{"batchLIFO 64B x512", K_BATCH_LIFO, 64, 0, 512, 300},
{"batchLIFO 256B x512", K_BATCH_LIFO, 256, 0, 512, 300},
{"batchLIFO 1024B x512", K_BATCH_LIFO, 1024, 0, 512, 300},
{"batchLIFO 4096B x512", K_BATCH_LIFO, 4096, 0, 512, 150},
{"batchFIFO 16B x512", K_BATCH_FIFO, 16, 0, 512, 300},
{"batchFIFO 64B x512", K_BATCH_FIFO, 64, 0, 512, 300},
{"batchFIFO 256B x512", K_BATCH_FIFO, 256, 0, 512, 300},
{"batchFIFO 1024B x512", K_BATCH_FIFO, 1024, 0, 512, 300},
{"deepLIFO 64B x4096", K_BATCH_LIFO, 64, 0, 4096, 40},
{"deepFIFO 64B x4096", K_BATCH_FIFO, 64, 0, 4096, 40},
/* calloc (n, size) */
{"calloc 1x64", K_CALLOC, 64, 0, 1, 200000},
{"calloc 8x8", K_CALLOC, 8, 0, 8, 200000},
{"calloc 128x4", K_CALLOC, 4, 0, 128, 150000},
{"calloc 256x16", K_CALLOC, 16, 0, 256, 80000},
{"calloc 1024x8", K_CALLOC, 8, 0, 1024, 40000},
{"calloc 1x64KB", K_CALLOC, 65536, 0, 1, 20000},
/* realloc growth chains (start a, double n times) */
{"realloc 16->16KB", K_REALLOC, 16, 0, 10, 20000},
{"realloc 8->128KB", K_REALLOC, 8, 0, 14, 8000},
{"realloc 64->4KB", K_REALLOC, 64, 0, 6, 60000},
{"realloc 32->8KB", K_REALLOC, 32, 0, 8, 30000},
/* memalign (size a, alignment b) */
{"memalign a16 s64", K_MEMALIGN, 64, 16, 0, 150000},
{"memalign a64 s256", K_MEMALIGN, 256, 64, 0, 120000},
{"memalign a256 s1024", K_MEMALIGN, 1024, 256, 0, 80000},
{"memalign a4096 s128", K_MEMALIGN, 128, 4096, 0, 80000},
{"memalign a4096 s4096", K_MEMALIGN, 4096, 4096, 0, 40000},
/* randomized mixed-size churn (min a, max b, slots n) */
{"mixed tiny 1-128", K_MIXED, 1, 128, 256, 400000},
{"mixed small 129-1024", K_MIXED, 129, 1024, 256, 300000},
{"mixed large 1K-16K", K_MIXED, 1025, 16384, 128, 120000},
{"mixed full 1-16K", K_MIXED, 1, 16384, 512, 200000},
};

#endif

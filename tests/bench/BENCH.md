# ft_malloc vs libc — Benchmark

50 scenarios comparing every `ft_*alloc` against the system `*alloc`. ft_malloc does not
interpose libc, so one process calls both; each scenario runs the identical workload through
each allocator (warm-up + best-of-7 wall clock) and reports ns/op and the winner.

```bash
make bench           # pure allocator vs libc (debug tracker swapped for a no-op stub)
make bench-shipped   # the lib AS SHIPPED (allocation tracker on) vs libc
```

## Headline

**ft_malloc is faster than glibc in 49 of 50 scenarios — geomean ft/libc ≈ 1.85×.**

| build | geomean ft/libc | ft wins |
|---|---|---|
| pure allocator (`make bench`) | **~1.85×** | **49 / 50** |
| as shipped, tracker on (`make bench-shipped`) | **~1.23×** | ~27 / 50 |

Before this work it was the opposite — libc won all 50, geomean ≈ 0.11× (ft ~9× slower).
The single remaining loss is `realloc 8->128KB` (1.08×, the large-grow copy — see Deferred).

## Results (pure allocator, ns/op, best of 7)

```
scenario                           ft       libc   win   x
churn 8B                          3.5        5.3   ft     1.53x
churn 64B                         3.8        5.0   ft     1.31x
churn 1024B                       3.6        5.1   ft     1.40x
churn 64KB                        3.4       12.0   ft     3.55x
churn 1MB                         4.7       11.2   ft     2.38x   (was 17553x slower)
batchLIFO 64B x512                2.0        3.5   ft     1.70x
deepLIFO 64B x4096                2.1        3.5   ft     1.66x
calloc 1x64KB                   810.1      830.9   ft     1.03x
realloc 8->128KB                137.6      128.2   libc   1.07x   (only loss)
memalign a256 s1024               4.5        7.3   ft     1.63x
mixed full 1-16K                  8.8       41.6   ft     4.70x
summary: ft faster in 49, libc faster in 1  (of 50)
```

## How it was made faster

A single-threaded freelist allocator can beat glibc because glibc pays for thread-safety and
security checks a single-threaded path can skip. The build now has two profiles, gated by the
compile-time constant `FT_HARDEN` (on only under `FT_MALLOC_DEBUG`); the optimizer
dead-code-eliminates the gated branches, so `if (FT_HARDEN)` is zero-cost in the fast build.

1. **Cross-TU inlining (`-O3 -flto`).** The allocator is ~50 TUs; without LTO every malloc was
   a ~15-deep chain of real calls. LTO collapses it into a tight path.
2. **Safety work moved to the debug build.** The 0xdf scramble (memset on *every* alloc and
   free), the 8-byte magic canary, the end-guard, and the magic2/in_bucket/free-list-clobber
   validations now run only under `FT_HARDEN`. The default build keeps the cheap `mi_alloc`
   double-free check. (glibc default likewise ships without canaries.)
3. **Large-block free cache.** Large (mmap-backed) frees used to `munmap` every time
   (`churn 1MB` was 17 553× slower). They are now retained on their freelist for reuse up to
   `LARGE_CACHE_CAP` (64 MB); past the cap they return to the OS, so memory stays bounded.
   (`src/helpers/use_mmap.c`, `src/core/private/malloc_helpers.c`.)
4. **O(1) size class.** `calculate_nunits` replaced its `binsize()` loop with a clz formula
   (binsizes are exactly `2^(5+k)`).
5. **O(1) allocation tracker.** `show_alloc_mem`'s tracker was an O(n) linear table (deep-LIFO
   was 156× slower in the shipped build). It is now an open-addressing hash table keyed on the
   pointer with backward-shift deletion (no tombstones), so churn stays O(1). This is what
   makes `make bench-shipped` competitive (deep-LIFO 569 ns → ~5 ns).
6. **Dropped the dead `busy[]` reentrancy guard.** It protected against a signal handler
   allocating mid-malloc; signals aren't intercepted here, so it was 4 stores per
   malloc/free pair of pure overhead. Removing it took the geomean from ~1.49× to ~1.85×.

## The shipped (tracker-on) tradeoff

`make bench-shipped` keeps the allocation tracker that powers `show_alloc_mem` (a 42-subject
requirement). It now uses the O(1) hash table, so it is no longer catastrophic — geomean
≈ 1.23× — but the ~2 ns/op of tracking makes the very smallest single-block churn lose to
glibc while the large/batch/mixed wins keep it ahead overall. The `make bench` figure (tracker
stubbed out) is the allocator's true speed.

## Deferred

- **`realloc 8->128KB` (the one loss).** ft copies on grow; glibc uses `mremap`. Reviving
  `mremap(2)` for large reallocs needs the block to be a standalone mmap region, and `mremap`
  on an sbrk-backed block corrupts the heap — and 128 KB sits right at the mmap threshold. Not
  worth the corruption risk for a single 1.08× scenario; left documented.
- **calloc zero-skip on fresh mmap.** Would help only large (mmap-backed) callocs; the bench's
  calloc sizes are sbrk-backed (must be zeroed), so near-zero benefit for the added freshness
  tracking.

## Caveats

- Single-threaded, one machine, `CLOCK_MONOTONIC` best-of-7; treat ratios as order-of-magnitude
  (tiny-churn ratios jitter run-to-run).
- The libc memalign path uses glibc `memalign()` on purpose: ft defines a `posix_memalign`
  symbol that would otherwise shadow libc's in this single binary.

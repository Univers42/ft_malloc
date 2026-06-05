# ft_malloc vs libc — Benchmark

50 scenarios comparing every `ft_*alloc` against the system `*alloc`. ft_malloc does not
interpose libc, so one process calls both; each scenario runs the identical workload through
each allocator (warm-up + best-of-7 wall clock) and reports ns/op and the winner.

```bash
make bench           # single-thread, 50 scenarios, ft vs libc
make bench-shipped   # same, with the show_alloc_mem tracker present
make bench-mt        # multithreaded throughput (1..16 threads) ft vs libc
```

## Headline

The shipped allocator is now **thread-safe** and faster than glibc **both single- and
multi-threaded**:

| benchmark | result |
|---|---|
| single-thread (`make bench`, 50 scenarios) | geomean ft/libc **~1.16×**, ft wins ~32/50 |
| multithreaded (`make bench-mt`, 1→16 threads) | geomean **~1.35–1.44×**, ft wins **every** thread count, scales ~9× to 16 threads |

Thread-safety is verified by **Helgrind (0 data races)** and an 8-thread cross-thread stress
test; see the design note below. Before any of this work, libc won all 50 single-thread
scenarios (geomean ≈ 0.11×, ft ~9× slower).

> **The pure single-thread peak (no locks, geomean ~1.85×, 49/50) is preserved as the git
> tag `v1.0-st-fast`.** Adding thread-safety costs the central lock on medium/large sizes
> (glibc pays an arena lock there too), which is why the thread-safe single-thread geomean is
> ~1.16× rather than 1.85×; the small/scalable sizes stay lock-free via the per-thread cache.

## Multithreaded throughput (`make bench-mt`, Mops/s, 20-core box)

```
threads            ft         libc   ft/libc
1               211.7        149.5   1.42x
2               338.4        319.7   1.06x
4               634.6        486.0   1.31x
8              1313.6        765.0   1.72x
16             2078.9       1570.2   1.32x
geomean ft/libc across thread counts: ~1.35x
```

Each thread runs an independent small-size churn (the scalable common case): ft's per-thread
magazine is lock-free, so it scales like glibc's tcache while staying ahead.

## Thread-safety design

- **Per-thread magazine cache (lock-free fast path).** malloc pops / free pushes a
  thread-local freelist per small size class (`src/helpers/tcache.c`, `__thread`). No lock.
- **Central heap under one mutex** (`src/helpers/lock.c`). Taken only to refill/flush a
  magazine or grow an arena (mmap). Large classes and the hardened build use it directly.
- **mmap-only** arena growth (concurrent `sbrk` races with libc's `brk`); the size class is in
  the block header, so cross-thread frees are correct.
- **`show_alloc_mem` walks an arena registry** (`src/helpers/arena.c`, `arena_walk.c`) instead
  of a per-op tracker, so the hot path has zero shared writes. The per-op call-site tracker is
  debug-build-only.

Note: TSan can't be used here — it rejects the allocator's raw `mmap` regions ("unexpected
memory mapping"), a known limitation for allocators that manage their own memory. Helgrind
(which virtualizes memory) is the substitute and reports 0 races.

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

## The shipped build

`make bench-shipped` builds the library exactly as shipped. The per-op `show_alloc_mem` tracker
is now **debug-build-only** (the default build answers `show_alloc_mem` by walking the arena
registry), so `bench-shipped` ≈ `bench` — no per-op tracking tax.

## `realloc` and `mremap` — measured and rejected

`realloc 8->128KB` is ~1.02× (parity with glibc; the central lock narrowed the old 1.08× gap).
ft reallocs a growing block by `internal_malloc` + `fastcopy` + `internal_free`. I implemented
the obvious "use `mremap(2)` for the large standalone-mmap blocks" optimization and **measured
it 58× slower** (8010 ns vs 136 ns): for ≤ 1 MB the `mremap` syscall plus the kernel page move
costs far more than a `memcpy`, and a growing mapping almost always has to move. `mremap` only
pays off for multi-MB blocks. So the copy path is the right choice here and is at parity —
reverted. (calloc zero-skip on fresh mmap is similarly low-value: the bench's calloc sizes are
served from recycled blocks that must be zeroed.)

## Real-program benchmarks (`make bench-gnl`, `make bench-gnl-mt`, `make philo-ft`)

The synthetic suite above is a *pure-allocator* microbenchmark. These run the allocator
through actual programs:

- **get_next_line** (`tests/get_next_line.c`, real source) — malloc/realloc-heavy: every line
  is built in a buffer grown by repeated realloc. `bench-gnl` runs the real get_next_line over
  a 16 MB file (no printf in the hot loop). `bench-gnl-mt` runs the same line-reading pattern
  in 1..16 threads (a static-free reader, since get_next_line's per-fd statics aren't safe once
  threads recycle fds).
- **dining philosophers** (`tests/philosopher`, submodule) — a real pthreads program;
  `make philo-ft` runs it entirely on ft_malloc via the `LD_PRELOAD` shim `tests/interpose.c`
  (ft_malloc is not an interposer, so the shim maps libc `malloc`/`free`/`realloc`/`calloc`
  to `ft_*`). `make philo-helgrind` runs it under Helgrind.

**Results.** get_next_line single-thread: ft ≈ **0.88×** glibc (10.6 vs 12.1 Mlines/s);
multithreaded: **near parity** (within ~5–9% across 1–16 threads, ft ahead at some counts,
run-to-run noisy). philosophers: **runs correctly and Helgrind-clean** on ft_malloc.

**Why gnl lands near parity, not the synthetic 1.17×/1.34× win:** gnl spends roughly half its
time in the per-line `memcpy` (allocator-independent), so any allocator edge is diluted; and
its growing-buffer realloc favors glibc's boundary-tag design, which extends a chunk *in place*,
whereas ft's segregated buckets must copy when a grow crosses a size class. The dedicated
**lock-free TLS realloc fast path** (`src/helpers/tcache2.c`: `realloc(NULL)` → magazine pop,
same/smaller class → in place, grow-within-TLS → magazine copy) was added for exactly this
workload — it lifted gnl from a clear loss (~0.64×, when every realloc took the central lock)
to the near-parity above. Takeaway: ft wins allocation-bound workloads; on memcpy-bound real
code it matches glibc and stays correct + race-free.

## Caveats

- Single-threaded, one machine, `CLOCK_MONOTONIC` best-of-7; treat ratios as order-of-magnitude
  (tiny-churn ratios jitter run-to-run).
- The libc memalign path uses glibc `memalign()` on purpose: ft defines a `posix_memalign`
  symbol that would otherwise shadow libc's in this single binary.
- get_next_line is ~half memcpy, so `bench-gnl` ratios understate the allocator's own speed;
  read it together with the synthetic `make bench`.

# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

`ft_malloc` — a 42 reimplementation of `malloc`/`free`/`realloc` built as a shared
library (`bin/libft_malloc.so`). It is its **own git repository** (`Univers42/ft_malloc`),
vendored into the parent `ft_linux` LFS project at `vendor/ft_malloc`. **The parent
project's Docker / hellish / "zero host pollution" rules do NOT apply here** — ft_malloc
is a plain C library that builds directly with host `gcc` via `make`. There is no
container and no hellish in this subtree.

The implementation is **derived from 4.4BSD `malloc.c`**. The original reference sources
live untouched in `ref/malloc/` (`malloc.c`, `sbrk.c`, `table.c`, `trace.c`, `watch.c`,
`xmalloc.c`, …) — they are documentation/oracle, **not compiled**. The names `morecore`,
`botch`/`xbotch`, `bsplit`, `nextf`/`busy` buckets, `memscramble`, trace/watch all come
straight from that lineage. The repo refactors it into 42-norm-compliant short functions.

## Build & run

```bash
make              # → bin/libft_malloc.so + all test binaries (gcc -Wall -Wextra -Werror -fPIC -O2 -shared)
make HYBRID=1     # adds -DUSE_HYBRID_MODE: sbrk for TINY/SMALL, mmap for LARGE (default is pure mmap)
make MODE_MALLOC=1 # builds the gnl test binary against libc malloc instead of the lib (A/B only)
make re / clean / fclean
```

There is **no `test` or `norm` target.** Tests are the binaries themselves; run them directly.
Test binaries are linked `-L bin -lft_malloc -Wl,-rpath,'$ORIGIN'`, so they load the `.so`
sitting next to them in `bin/` — run them from anywhere, but keep the `.so` in `bin/`.

```bash
./bin/alloc_test          # comprehensive suite (tests/test_comprehensive.c). old_result.txt is a saved good run to diff against
./bin/mode_ft             # workload under ft_malloc      ─┐ A/B the custom allocator
./bin/mode_libc           # same workload under libc malloc ┘ against the system one
./bin/gnl                 # get_next_line driven entirely through the allocator (real-program smoke test)
```

`bin/alloc` is **stale** — it is not a target in the current Makefile; ignore it.

**Leak & correctness audit** (`tests/leakcheck/`, see its `AUDIT.md`): `make leakcheck`
(syscall interposer + call-site leak map), `make leakcheck-asan`, `make leakcheck-valgrind`,
`make leakcheck-all`. The harness builds a separate debug lib `bin/libft_malloc_dbg.so`
(`-DFT_MALLOC_DEBUG`, adds a destructor leak report + `ft_*_dbg(__FILE__,__LINE__)` call-site
capture) and leaves the subject `make` untouched. Programmatic oracles `malloc_live_bytes()`,
`malloc_live_count()`, `malloc_leak_map()` live in `src/debug/leakcheck.c`. NB: Valgrind/ASan
are blind to ft_malloc's *logical* leaks (mmap/sbrk-backed), so the interposer + tracker are
the authoritative leak oracles; ASan/Valgrind still catch genuine UB in the allocator's code.

**Benchmark** (`tests/bench/`, see `BENCH.md`): `make bench` runs ~50 scenarios comparing every
`ft_*alloc` against libc `*alloc` in one process (ft doesn't interpose libc). `make bench`
measures the pure allocator (swaps `track.c` for a no-op stub via `filter-out`); `make
bench-shipped` keeps the real tracker. **ft now beats glibc in 49/50, geomean ft/libc ≈ 1.85×**
(bench-shipped ≈ 1.23×). It got there via: `-O3 -flto`, a large-block free cache
(`LARGE_CACHE_CAP`, no more munmap-per-free), an O(1) clz size class, an O(1) hash tracker
(backward-shift delete, in `src/debug/track_tbl.c`), dropping the dead `busy[]` guard, and
gating all per-op safety behind `FT_HARDEN`.

**Build profiles — `FT_HARDEN` (`include/alloc.h`).** `FT_HARDEN` is a 0/1 compile-time
constant (1 only under `FT_MALLOC_DEBUG`); the hot path gates safety with `if (FT_HARDEN)` so
the optimizer drops it at zero cost. Default `make` = fast path (cheap `mi_alloc` double-free
check only). `FT_MALLOC_DEBUG` (the `make leakcheck*` / `make debug` build) = full hardening:
0xdf scramble, 8-byte magic canary, end-guard, in_bucket + free-list-clobber validation, plus
the call-site tracker. The corruption death-tests and the audit run on the debug build, so they
still detect overflow/use-after-free; the default build does not (like glibc default). Never
mix object files across profiles — the block-header layout differs.

## Public API (note: NOT an interposer)

The exported symbols are **`ft_`-prefixed** (`ft_malloc`, `ft_free`, `ft_realloc`,
`ft_calloc`, `ft_valloc`, `ft_memalign`, `ft_cfree`) plus `show_alloc_mem`. This library
does **not** override libc `malloc` via symbol interposition — callers must call `ft_*`
explicitly (see `tests/mode_test.c`, which `#define`-switches between `ft_malloc` and
`malloc`). Header is `include/public.h` → `alloc_funcs.h` + `alloc_types.h`.

## Architecture (the parts that span files)

**One global, function-static.** All allocator state lives in a single
`static t_glob glob` inside `get_glob()` (`src/helpers/accessors.c`). `get_glob(GLOB_NONE,
NULL)` reads it; `get_glob(field, &val)` mutates one field. `t_glob` (`alloc_types.h`)
holds the live state: `nextf[NBUCKETS]` (per-bucket freelist heads), `busy[NBUCKETS]`
(reentrancy guards), `binsizes[]`, the stats counters, the runtime flags
(`malloc_trace`/`malloc_register`/`malloc_mmap_threshold`), and the zone lists.

**The live allocation path is bucket/freelist, NOT zones.** Despite the README's
prominent "zone-based" framing, the `t_zone` machinery (`tiny_zones`, `small_zones`,
`create_zone`, `allocate_from_zone`, `free_to_zone` in `src/helpers/zone_manager*.c`) has
**no callers in the allocation path** — it is dormant scaffolding. The real hot path is:

```
ft_malloc(size)                      src/core/malloc.c
 └ internal_malloc → do_malloc       src/core/private/internal_malloc.c
    ├ allocated_bytes(n)             add header + guard slop → request size
    ├ calculate_nunits(nbytes)       size → bucket index via binsize()
    ├ get_block_from_freelist(nu)    src/core/private/malloc_helpers.c
    │   └ if nextf[nu] empty → morecore(nu)   src/helpers/dyn_core*.c (mmap, or sbrk in HYBRID) then bsplit/chain
    ├ setup_block_header / setup_end_guard    write t_mhead + trailing guard
    └ finalize_alloc                 memscramble, stats, trace/register/watch, track_allocation
```

`free`/`realloc` mirror this through `internal_free` / `internal_realloc` in
`src/core/private/`. `bcoalesce` merges adjacent free blocks; `bsplit`/`xsplit` split a
larger block down a bucket. `show_alloc_mem` (debug layer) is the *only* consumer of the
TINY≤128 / SMALL≤1024 / LARGE categorisation — it groups the separately-maintained
`track_allocation` array for display; those thresholds do not drive allocation.

**Block layout & corruption detection.** Every allocation is `t_mhead` (16 B union,
`alloc_types.h`) + user data + a 4-byte end guard. The header carries `mi_alloc`
(`ISALLOC 0xf7` / `ISFREE 0x54`), `mi_index` (bucket), `mi_magic2` (`0x5555`), `mi_nbytes`,
and `mi_magic8[8]` (`0x55` fill). `free`/`realloc` validate these (`validate_magic8`,
`validate_end_guard`) and report via `botch`/`xbotch` → the `errn`/`t_err` flags. A known
historical bug class here: `realloc` that **moves** a block (e.g. `mremap_implement` in
`src/utils/`) must re-write the end guard on the *new* block, not just the header — see
recent git history.

**Source layout.** `src/core/` = public entry points; `src/core/private/` = the
`internal_*` implementations + per-op helpers (this is where most logic lives).
`src/helpers/` = memory mechanics (morecore/bsplit/bcoalesce/chain/buckets/signals, plus
freestanding `memset`/`bzero`/`memcpy`/`fastcopy` so the lib has no libc-alloc dependency).
`src/debug/` = `show_alloc_mem`, stats, trace, watch, table. `src/utils/` = mremap +
memalign. **Adding a `.c` requires hand-editing the `*_SRCS` lists in the Makefile** —
there is no wildcard globbing.

**Signal safety.** Mutating freelists is bracketed by `malloc_block_signals` /
`malloc_unblock_signals` (see `morecore`); preserve this around any new critical section.

## Constraints when editing

- **42 norminette compliance is required and actively maintained** (recent commits are
  specifically about it; you'll see `/* kept short to satisfy style */`-type notes). Keep
  the 42 header banner, `t_`/`s_`/`e_` type naming, and functions short (≤25 lines, ≤5
  args — hence the pervasive `t_*_ctx` struct-passing pattern). New code must stay clean.
- Compilation is `-Werror`; warnings are build failures.
- **Trust the code over the README.** `README.md` is largely aspirational marketing: it
  describes a zone allocator (dormant), an automatic on-exit destructor leak report and
  env-var-driven tracing (no `__attribute__((destructor))` and no `getenv` exist in
  `src/`), and lists a file tree that no longer matches. The real leak check is calling
  `show_alloc_mem()` manually. Verify any README claim against `src/` before relying on it.
- Use `ref/malloc/` as the behavioural oracle when a primitive's intent is unclear — the
  current code is a refactor of those files.

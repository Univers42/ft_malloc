# ft_malloc — Leak & Correctness Audit

A full audit of `ft_malloc`/`ft_free`/`ft_realloc`/`ft_calloc`/`ft_valloc`/`ft_memalign`,
plus a reusable "inner tester" that proves leak-freeness and correctness. Because the
allocator gets memory straight from `mmap`/`sbrk`, Valgrind/ASan are blind to its *logical*
leaks, so the harness carries its own oracles.

## TL;DR

- **3 defects found, 2 fixed, 1 documented as latent.** The headline find is a **heap
  buffer overflow when freeing an aligned `ft_memalign` block** — silent memory corruption
  that crashed only later, surfaced by the churn lane and root-caused with ASan.
- Final state: **all four lanes PASS** (33/33 with the interposer; 30/30 under ASan and
  under Valgrind — the 3 OS-level checks are interposer-only). Subject build unchanged
  (`alloc_test` 27/27), `norminette` clean on `include/` + `src/`.

## How to run

```bash
make leakcheck            # Lane 1 (syscall interposer) + Lane 2 (call-site leak map)
make leakcheck-asan       # Lane 3: AddressSanitizer + UBSan
make leakcheck-valgrind   # Lane 4: Valgrind memcheck
make leakcheck-all        # everything + a leak-map demo
make                      # the subject build stays pure (no debug, no destructor)
```

## What was built

| Piece | Path | Role |
|---|---|---|
| Test battery | `tests/leakcheck/full_audit.c` | API×size matrix, realloc integrity, offset/alignment, churn ("bouncing"), double-free/corruption ("debounce"), OS-leak |
| Syscall interposer | `tests/leakcheck/preload_syscalls.c` → `bin/libsyscount.so` | `LD_PRELOAD` wraps mmap/munmap/mremap/sbrk; maps un-returned regions; weak hooks make OS-leak a hard assertion |
| Call-site macro layer | `tests/leakcheck/ft_malloc_debug.h` | `FT_MALLOC_DEBUG` routes `ft_malloc(...)`→`ft_malloc_dbg(...,__FILE__,__LINE__)` |
| Lane driver | `tests/leakcheck/run.sh` | runs/aggregates the lanes |
| Programmatic oracle | `src/debug/leakcheck.c` | `malloc_live_bytes()`, `malloc_live_count()`, `malloc_leak_map()`, `malloc_track_overflow()` |
| Call-site tracker | `src/debug/track.c`, `include/debug.h` | tracker entries now carry `file:line`+serial; overflow counter |
| Debug wrappers | `src/core/dbg_api.c`, `dbg_api2.c` | `ft_*_dbg` thin wrappers (always compiled, harmless extra symbols) |
| Auto report | `src/debug/leakcheck_dtor.c` | `__attribute__((destructor))` leak report — **only** under `-DFT_MALLOC_DEBUG` |

The default `make` is byte-for-byte the same allocator (the tracker accuracy fixes only
improve `show_alloc_mem`); the destructor, macro layer, and `bin/libft_malloc_dbg.so`
exist only in the debug build.

## Two definitions of "leak"

1. **Logical** — an `ft_malloc`'d pointer never `ft_free`'d. Oracle: the tracker; at the end
   of every section `malloc_live_count()==0 && malloc_live_bytes()==0`.
2. **OS** — memory `mmap`'d and never `munmap`'d even after the block was freed. Oracle: the
   interposer. Note the allocator deliberately **retains `sbrk` arenas** for small buckets as
   a pool — that is *not* a leak. LARGE blocks are `mmap`'d 1:1 and `munmap`'d on free, which
   the harness asserts directly.

## Findings

### BUG #3 — heap buffer overflow + phantom leak on `ft_free` of an aligned `ft_memalign` (HIGH, fixed)

`ft_memalign(4096, 100)` over-allocates a base block, returns an *aligned* pointer inside it,
and drops an `ISMEMALILGN` marker just before the aligned pointer. On free,
`handle_memalign()` (`src/core/private/free_utils2.c`) recovered the base header into
`*ap`/`*p` but had `(void)mem;` — it **never updated `*mem`**. Two consequences:

- `ifree_handle_free()` then called `scramble_allocated_memory(c->mem, mi_nbytes)` with the
  *aligned* pointer and the *base* size (e.g. memset 4196 bytes starting `slack` bytes into
  the block) → **wrote past the block end, corrupting the adjacent heap block's header.** The
  process didn't crash there; it crashed later (e.g. during subsequent churn) with
  "free list clobbered" or a segfault. ASan pinned it instantly:

  ```
  WRITE of size 4196 in scramble_allocated_memory
    #1 malloc_memset            src/helpers/memset.c:50
    #2 scramble_allocated_memory src/core/private/malloc_triggers.c:18
    #3 ifree_handle_free        src/core/private/internal_free.c:40
    #6 (ft_free of an ft_memalign block)
  ```
- Separately, `internal_free()` untracked the raw parameter (aligned ptr) instead of the
  recovered base, so the tracker never cleared the entry → **phantom leak** in
  `show_alloc_mem`.

**Fix:** `handle_memalign()` now sets `*mem = (t_addr)(*p + 1)` (the recovered base), and
`internal_free()` untracks `ctx.mem` (post-recovery) instead of the stale parameter. Both the
overflow and the phantom leak vanish. **Regression:** the memalign section asserts
`malloc_live_count()==0` after free; the churn lane (which previously crashed) is clean under
the interposer, ASan, and Valgrind.

### BUG #1 — in-place `ft_realloc` drifts the live-byte total (LOW, fixed)

`realloc_in_place()` (`src/core/private/internal_realloc.c`) updated the block header
(`mi_nbytes`) + end guard but not the tracker, so after a same/adjacent-bucket realloc
`show_alloc_mem`'s "Total" (and class bucketing) was wrong. Leak *count* detection still held
(untrack matches on pointer), so this was a reporting-accuracy bug, not a memory bug.
**Fix:** re-sync the tracker (`untrack` + `track_allocation_dbg`) on the in-place path.
**Regression:** `malloc(8)`→`realloc(.,12)` asserts `live_bytes` becomes 12 (was stuck at 8).

### BUG #2 — stranded `sbrk` bytes on a misaligned grow (LATENT, documented)

`handle_sbrk()` (`src/helpers/dyn_core_helper.c`) bumps `g->memtop` by the full `sbrk`
amount, then, if the returned address is not 16-aligned, rounds up and drops a block; if that
makes `nblks <= 0` it returns failure with those bytes never chained → stranded. **In
practice this branch is unreachable**: every `sbrk` amount is a page multiple (`g->pagesz` or
rounded up to one), so the brk stays page-aligned (hence 16-aligned) and the misalignment
branch is never taken. The interposer confirms it: `sbrk` calls only ever move the brk by
page multiples (net brk delta a multiple of 4096), and no churn workload triggers it. Left
as-is to avoid perturbing the hot path; **recommended hardening**: advance `g->memtop` only by
the bytes actually placed on a freelist, or chain the alignment remainder.

## Lane results (final)

| Lane | What it proves | Result |
|---|---|---|
| 1 — syscall interposer | LARGE alloc `mmap`s one region; LARGE free `munmap`s it; no region leaked since baseline | PASS |
| 2 — call-site leak map | every live block reported as `size  file:line  (class)`; demo pinpoints leaks to `full_audit.c:360-362` | PASS |
| 3 — ASan + UBSan | no OOB/UB in the allocator's own code across all workloads (this is what caught BUG #3) | PASS, 0 errors |
| 4 — Valgrind memcheck | no invalid access / uninitialised use in allocator logic | PASS, 0 errors |

Coverage in `full_audit.c`: malloc/free across `0,1,…,1 MB`; calloc zeroing + `n*s`; realloc
grow/shrink/NULL/0 with byte-preservation; `ft_memalign`/`ft_valloc` alignment; header/guard
**offset** geometry (16-byte alignment, header at `−sizeof(t_mhead)`, magic2/ISALLOC); 40k-op
randomized **bouncing** churn with live-count drift detection; **debounce** death tests
(double-free, magic8 underflow, end-guard overflow, interior-pointer free → all must
`abort()`), and `free(NULL)` as a safe no-op.

## Known limitation (by design)

The tracker is single-threaded and capped at `MAX_TRACKED_ALLOCS` (10000) live entries; a
`malloc_track_overflow()` counter makes saturation observable instead of silent. For aligned
`ft_memalign` the tracker keys the *base* pointer and the over-allocated size (correct for
leak detection; the byte total is high by `alignment` while the block is live).

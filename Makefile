# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/25 14:48:54 by dlesieur          #+#    #+#              #
#    Updated: 2026/06/05 00:32:28 by dlesieur         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft_malloc.so
BINDIR = bin
TESTSRC = tests/get_next_line.c
TESTBIN = $(BINDIR)/gnl

CC = gcc
CFLAGS = -Wall -Wextra -Werror -fPIC -O3 -flto -pthread
INCLUDES = -I./include
LDFLAGS = -shared -flto -O3 -pthread

# Directories
SRC_DIR = src
OBJ_DIR = obj
CORE_DIR = $(SRC_DIR)/core
PRIVATE_DIR = $(CORE_DIR)/private
HELPERS_DIR = $(SRC_DIR)/helpers
DEBUG_DIR = $(SRC_DIR)/debug
UTILS_DIR = $(SRC_DIR)/utils

# Source files
CORE_SRCS = $(CORE_DIR)/malloc.c \
            $(CORE_DIR)/free.c \
            $(CORE_DIR)/realloc.c \
            $(CORE_DIR)/valloc.c\
            $(CORE_DIR)/calloc.c \
            $(CORE_DIR)/dbg_api.c \
            $(CORE_DIR)/dbg_api2.c \
            $(PRIVATE_DIR)/free_utils.c\
            $(PRIVATE_DIR)/free_utils2.c\
            $(PRIVATE_DIR)/malloc_helpers.c\
            $(PRIVATE_DIR)/malloc_triggers.c\
            $(PRIVATE_DIR)/malloc_triggers2.c\
            $(PRIVATE_DIR)/internal_calloc.c\
            $(PRIVATE_DIR)/internal_free.c\
            $(PRIVATE_DIR)/internal_malloc.c\
            $(PRIVATE_DIR)/internal_realloc.c\
            $(PRIVATE_DIR)/internal_valloc.c\
            $(PRIVATE_DIR)/internals.c

HELPERS_SRCS = $(HELPERS_DIR)/accessors.c \
                $(HELPERS_DIR)/lock.c \
                $(HELPERS_DIR)/arena.c \
                $(HELPERS_DIR)/tcache.c \
                $(HELPERS_DIR)/tcache2.c \
                $(HELPERS_DIR)/bcoalesce.c \
                $(HELPERS_DIR)/bcoalesce_helpers.c \
                $(HELPERS_DIR)/bsplit.c \
                $(HELPERS_DIR)/botch.c \
                $(HELPERS_DIR)/buckets.c \
                $(HELPERS_DIR)/chain.c \
                $(HELPERS_DIR)/dyn_core.c \
                $(HELPERS_DIR)/init.c \
                $(HELPERS_DIR)/pagealign.c \
                $(HELPERS_DIR)/signals.c \
                $(HELPERS_DIR)/find_adjacent.c \
                $(HELPERS_DIR)/singletons.c \
                $(HELPERS_DIR)/xsplit.c \
                $(HELPERS_DIR)/bsplit_helpers.c \
                $(HELPERS_DIR)/dyn_core_helper.c \
                $(HELPERS_DIR)/fastcopy.c \
                $(HELPERS_DIR)/use_mmap.c \
                $(HELPERS_DIR)/use_less_core.c \
                $(HELPERS_DIR)/zero.c \
                $(HELPERS_DIR)/bzero.c \
                $(HELPERS_DIR)/memset.c \
                $(HELPERS_DIR)/zone_manager_helps.c \
                $(HELPERS_DIR)/pagealign_helpers.c \
                $(HELPERS_DIR)/malloc_block_signals.c\
                $(HELPERS_DIR)/malloc_unblock_signals.c\
                $(HELPERS_DIR)/get_page_size.c\
                $(HELPERS_DIR)/malloc_register.c\
                $(HELPERS_DIR)/malloc_trace.c\
                $(HELPERS_DIR)/malloc_stats.c\
                $(HELPERS_DIR)/malloc_watch.c\
                $(HELPERS_DIR)/memscramble.c\
                $(HELPERS_DIR)/use_more_core.c\
                $(HELPERS_DIR)/zone_manager.c

DEBUG_SRCS = $(DEBUG_DIR)/stats.c \
                $(DEBUG_DIR)/helper.c \
                $(DEBUG_DIR)/show_alloc.c \
                $(DEBUG_DIR)/show_alloc2.c \
                $(DEBUG_DIR)/track.c \
                $(DEBUG_DIR)/track_tbl.c \
                $(DEBUG_DIR)/arena_walk.c \
                $(DEBUG_DIR)/leakcheck.c \
                $(DEBUG_DIR)/leakcheck_dtor.c \
                $(DEBUG_DIR)/stat_utils.c \
                $(DEBUG_DIR)/stat_utils2.c \
                $(DEBUG_DIR)/stat_utils3.c \
                $(DEBUG_DIR)/stat_utils4.c \
                $(DEBUG_DIR)/table.c \
                $(DEBUG_DIR)/trace.c \
                $(DEBUG_DIR)/watch.c

UTILS_SRCS = $(UTILS_DIR)/utils.c \
            $(UTILS_DIR)/ft_mremap.c \
            $(UTILS_DIR)/ft_memalign.c \
            $(UTILS_DIR)/mremap_internal.c

SRCS = $(CORE_SRCS) $(HELPERS_SRCS) $(DEBUG_SRCS) $(UTILS_SRCS)

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Colors
GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

# Compilation modes
# Default: pure mmap (for 42 project)
# Hybrid: sbrk for small, mmap for large (optimized)
ifdef HYBRID
	CFLAGS += -DUSE_HYBRID_MODE
endif

# Test mode program
MODE_TEST_SRC = tests/mode_test.c
MODE_FT_BIN = $(BINDIR)/mode_ft
MODE_LIBC_BIN = $(BINDIR)/mode_libc

# Comprehensive test
COMP_TEST_SRC = tests/test_comprehensive.c
COMP_TEST_BIN = $(BINDIR)/alloc_test

# ---- Leak-audit harness (debug build; NOT part of the subject deliverable) ----
DBG_NAME    = libft_malloc_dbg.so
DBG_OBJ_DIR = obj_dbg
DBG_OBJS    = $(SRCS:$(SRC_DIR)/%.c=$(DBG_OBJ_DIR)/%.o)
DBG_CFLAGS  = -Wall -Wextra -Werror -fPIC -g -DFT_MALLOC_DEBUG -pthread
LEAK_DIR    = tests/leakcheck
LEAK_SRC    = $(LEAK_DIR)/full_audit.c
LEAK_BIN    = $(BINDIR)/full_audit
ASAN_BIN    = $(BINDIR)/full_audit_asan
SYSCOUNT    = $(BINDIR)/libsyscount.so
LEAK_CFLAGS = -Wall -Wextra -g -DFT_MALLOC_DEBUG -pthread

# ---- Benchmark (ft_* vs libc) ----------------------------------------------
BENCH_DIR     = tests/bench
BENCH_SRC     = $(BENCH_DIR)/bench.c
BENCH_BIN     = $(BINDIR)/bench
BENCH_FULL    = $(BINDIR)/bench_shipped
BENCH_MT_SRC  = $(BENCH_DIR)/bench_mt.c
BENCH_MT_BIN  = $(BINDIR)/bench_mt
# pure-allocator build: swap the O(n) debug tracker for a no-op stub
BENCH_CORESRC = $(filter-out $(DEBUG_DIR)/track.c,$(SRCS)) $(BENCH_DIR)/notrack_stub.c
BENCH_CFLAGS  = -O3 -flto -Wall -Wextra -pthread

all: $(BINDIR)/$(NAME) $(TESTBIN) $(MODE_FT_BIN) $(MODE_LIBC_BIN) $(COMP_TEST_BIN)

$(BINDIR)/$(NAME): $(OBJS)
	@mkdir -p $(BINDIR)
	@echo "$(GREEN)Linking $@...$(RESET)"
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "$(GREEN)✓ $@ created successfully$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(GREEN)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TESTBIN): $(TESTSRC) $(BINDIR)/$(NAME)
	@mkdir -p $(BINDIR)
	@echo "$(GREEN)Compiling test $(TESTSRC) -> $(TESTBIN)...$(RESET)"
	@if [ "$(MODE_MALLOC)" = "1" ]; then \
		$(CC) $(CFLAGS) -DMODE_MALLOC=1 -I./include -o $(TESTBIN) $(TESTSRC); \
	else \
		$(CC) $(CFLAGS) -I./include -o $(TESTBIN) $(TESTSRC) -L$(BINDIR) -lft_malloc -Wl,-rpath,'$$ORIGIN'; \
	fi

$(MODE_FT_BIN): $(MODE_TEST_SRC) $(BINDIR)/$(NAME)
	@mkdir -p $(BINDIR)
	@echo "$(GREEN)Compiling mode test (ft) $(MODE_TEST_SRC) -> $(MODE_FT_BIN)...$(RESET)"
	@$(CC) $(CFLAGS) -I./include -o $(MODE_FT_BIN) $(MODE_TEST_SRC) -L$(BINDIR) -lft_malloc -Wl,-rpath,'$$ORIGIN'

$(MODE_LIBC_BIN): $(MODE_TEST_SRC)
	@mkdir -p $(BINDIR)
	@echo "$(GREEN)Compiling mode test (libc) $(MODE_TEST_SRC) -> $(MODE_LIBC_BIN)...$(RESET)"
	@$(CC) $(CFLAGS) -DMODE_MALLOC=1 -I./include -o $(MODE_LIBC_BIN) $(MODE_TEST_SRC)

$(COMP_TEST_BIN): $(COMP_TEST_SRC) $(BINDIR)/$(NAME)
	@mkdir -p $(BINDIR)
	@echo "$(GREEN)Compiling comprehensive test -> $(COMP_TEST_BIN)...$(RESET)"
	@$(CC) $(CFLAGS) -I./include -o $(COMP_TEST_BIN) $(COMP_TEST_SRC) -L$(BINDIR) -lft_malloc -Wl,-rpath,'$$ORIGIN'

# ---- Leak-audit lanes ------------------------------------------------------
$(DBG_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(DBG_CFLAGS) $(INCLUDES) -c $< -o $@

$(BINDIR)/$(DBG_NAME): $(DBG_OBJS)
	@mkdir -p $(BINDIR)
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "$(GREEN)✓ $@ (debug lib) created$(RESET)"

$(SYSCOUNT): $(LEAK_DIR)/preload_syscalls.c
	@mkdir -p $(BINDIR)
	@$(CC) -Wall -Wextra -fPIC -g $(LDFLAGS) -o $@ $< -ldl
	@echo "$(GREEN)✓ $@ (syscall interposer) created$(RESET)"

$(LEAK_BIN): $(LEAK_SRC) $(BINDIR)/$(DBG_NAME)
	@mkdir -p $(BINDIR)
	@$(CC) $(LEAK_CFLAGS) $(INCLUDES) -o $@ $(LEAK_SRC) \
		-L$(BINDIR) -lft_malloc_dbg -Wl,-rpath,'$$ORIGIN'
	@echo "$(GREEN)✓ $@ (leak harness) created$(RESET)"

$(ASAN_BIN): $(SRCS) $(LEAK_SRC)
	@mkdir -p $(BINDIR)
	@$(CC) $(LEAK_CFLAGS) -fsanitize=address,undefined $(INCLUDES) \
		-o $@ $(SRCS) $(LEAK_SRC)
	@echo "$(GREEN)✓ $@ (ASan/UBSan harness) created$(RESET)"

debug: $(BINDIR)/$(DBG_NAME)

leakcheck: $(LEAK_BIN) $(SYSCOUNT)
	@$(LEAK_DIR)/run.sh lane12

leakcheck-asan: $(ASAN_BIN)
	@$(LEAK_DIR)/run.sh asan

leakcheck-valgrind: $(LEAK_BIN)
	@$(LEAK_DIR)/run.sh valgrind

leakcheck-all: $(LEAK_BIN) $(SYSCOUNT) $(ASAN_BIN)
	@$(LEAK_DIR)/run.sh all

$(BENCH_BIN): $(BENCH_SRC) $(BENCH_CORESRC) $(BENCH_DIR)/bench_scn.h
	@mkdir -p $(BINDIR)
	@$(CC) $(BENCH_CFLAGS) $(INCLUDES) -o $@ $(BENCH_SRC) $(BENCH_CORESRC) -lm
	@echo "$(GREEN)✓ $@ (pure allocator vs libc) created$(RESET)"

$(BENCH_FULL): $(BENCH_SRC) $(SRCS) $(BENCH_DIR)/bench_scn.h
	@mkdir -p $(BINDIR)
	@$(CC) $(BENCH_CFLAGS) $(INCLUDES) -o $@ $(BENCH_SRC) $(SRCS) -lm
	@echo "$(GREEN)✓ $@ (shipped lib incl. tracker vs libc) created$(RESET)"

$(BENCH_MT_BIN): $(BENCH_MT_SRC) $(BENCH_CORESRC) $(BENCH_DIR)/bench_scn.h
	@mkdir -p $(BINDIR)
	@$(CC) $(BENCH_CFLAGS) $(INCLUDES) -o $@ $(BENCH_MT_SRC) $(BENCH_CORESRC) -lm
	@echo "$(GREEN)✓ $@ (multithreaded ft vs libc) created$(RESET)"

bench: $(BENCH_BIN)
	@$(BENCH_BIN)

bench-shipped: $(BENCH_FULL)
	@$(BENCH_FULL)

bench-mt: $(BENCH_MT_BIN)
	@$(BENCH_MT_BIN)

# ---- Multithreaded race / stress verification ------------------------------
MT_BIN = $(BINDIR)/mt_stress

$(MT_BIN): tests/mt/mt_stress.c $(BINDIR)/$(NAME)
	@mkdir -p $(BINDIR)
	@$(CC) -O2 -pthread $(INCLUDES) -o $@ tests/mt/mt_stress.c \
		-L$(BINDIR) -lft_malloc -Wl,-rpath,'$$ORIGIN'
	@echo "$(GREEN)✓ $@ created$(RESET)"

mt-stress: $(MT_BIN)
	@LD_LIBRARY_PATH=$(BINDIR) $(MT_BIN) 8 200000

helgrind: $(MT_BIN)
	@LD_LIBRARY_PATH=$(BINDIR) valgrind --tool=helgrind --error-exitcode=99 -q \
		$(MT_BIN) 3 3000 && echo "$(GREEN)✓ Helgrind: 0 data races$(RESET)"

# ---- Real-program benchmark: get_next_line (malloc+realloc heavy) -----------
GNL_SRC      = tests/get_next_line.c
GNL_BENCH    = tests/bench/bench_gnl.c
GNL_MT       = tests/bench/bench_gnl_mt.c
GNL_FT       = $(BINDIR)/bench_gnl_ft
GNL_LIBC     = $(BINDIR)/bench_gnl_libc
GNL_MT_FT    = $(BINDIR)/bench_gnl_mt_ft
GNL_MT_LIBC  = $(BINDIR)/bench_gnl_mt_libc
GNL_CFLAGS   = -O2 $(INCLUDES) -Itests
RPATH        = -Wl,-rpath,'$$ORIGIN'

# get_next_line.c carries its own main(); rename it out so the bench main wins.
$(GNL_FT): $(GNL_BENCH) $(GNL_SRC) $(BINDIR)/$(NAME)
	@mkdir -p $(BINDIR)
	@$(CC) $(GNL_CFLAGS) -Dmain=gnl_file_main -c $(GNL_SRC) -o $(BINDIR)/gnl_ft.o
	@$(CC) $(GNL_CFLAGS) -o $@ $(GNL_BENCH) $(BINDIR)/gnl_ft.o \
		-L$(BINDIR) -lft_malloc $(RPATH)

$(GNL_LIBC): $(GNL_BENCH) $(GNL_SRC)
	@mkdir -p $(BINDIR)
	@$(CC) $(GNL_CFLAGS) -DMODE_MALLOC=1 -Dmain=gnl_file_main -c $(GNL_SRC) \
		-o $(BINDIR)/gnl_libc.o
	@$(CC) $(GNL_CFLAGS) -DMODE_MALLOC=1 -o $@ $(GNL_BENCH) $(BINDIR)/gnl_libc.o

bench-gnl: $(GNL_FT) $(GNL_LIBC)
	@echo "$(GREEN)== get_next_line throughput (single thread) ==$(RESET)"
	@LD_LIBRARY_PATH=$(BINDIR) $(GNL_FT)
	@$(GNL_LIBC)

$(GNL_MT_FT): $(GNL_MT) $(BINDIR)/$(NAME)
	@mkdir -p $(BINDIR)
	@$(CC) $(GNL_CFLAGS) -pthread -o $@ $(GNL_MT) \
		-L$(BINDIR) -lft_malloc $(RPATH)

$(GNL_MT_LIBC): $(GNL_MT)
	@mkdir -p $(BINDIR)
	@$(CC) $(GNL_CFLAGS) -pthread -DMODE_MALLOC=1 -o $@ $(GNL_MT)

bench-gnl-mt: $(GNL_MT_FT) $(GNL_MT_LIBC)
	@echo "$(GREEN)== get_next_line throughput (multithreaded) ==$(RESET)"
	@LD_LIBRARY_PATH=$(BINDIR) $(GNL_MT_FT)
	@echo ""
	@$(GNL_MT_LIBC)

# ---- Concurrent server-pool benchmark (Larson-style, malloc/free-bound) -----
POOL_SRC   = tests/bench/bench_pool.c
POOL_FT    = $(BINDIR)/bench_pool_ft
POOL_LIBC  = $(BINDIR)/bench_pool_libc

$(POOL_FT): $(POOL_SRC) $(BINDIR)/$(NAME)
	@mkdir -p $(BINDIR)
	@$(CC) $(GNL_CFLAGS) -pthread -o $@ $(POOL_SRC) \
		-L$(BINDIR) -lft_malloc $(RPATH) -lm

$(POOL_LIBC): $(POOL_SRC)
	@mkdir -p $(BINDIR)
	@$(CC) $(GNL_CFLAGS) -pthread -DMODE_MALLOC=1 -o $@ $(POOL_SRC) -lm

bench-pool: $(POOL_FT) $(POOL_LIBC)
	@echo "$(GREEN)== concurrent server-pool: cross-thread malloc/free ==$(RESET)"
	@LD_LIBRARY_PATH=$(BINDIR) $(POOL_FT)
	@echo ""
	@$(POOL_LIBC)

# ---- Roll-up: every benchmark, synthetic + real-program ---------------------
bench-all:
	@echo "$(GREEN)#### 1/5  synthetic, single-thread (pure allocator) ####$(RESET)"
	@$(MAKE) -s bench
	@echo ""
	@echo "$(GREEN)#### 2/5  synthetic, multithreaded (pure allocator) ####$(RESET)"
	@$(MAKE) -s bench-mt
	@echo ""
	@echo "$(GREEN)#### 3/5  concurrent server pool (cross-thread malloc/free) ####$(RESET)"
	@$(MAKE) -s bench-pool
	@echo ""
	@echo "$(GREEN)#### 4/5  get_next_line, single-thread (real program) ####$(RESET)"
	@$(MAKE) -s bench-gnl
	@echo ""
	@echo "$(GREEN)#### 5/5  get_next_line, multithreaded (real program) ####$(RESET)"
	@$(MAKE) -s bench-gnl-mt
	@echo ""
	@echo "$(GREEN)bench-all done. Allocation-bound: ft wins (synthetic + pool).$(RESET)"
	@echo "$(GREEN)memcpy-bound (gnl): near parity. See tests/bench/BENCH.md.$(RESET)"

# ---- Real multithreaded program on ft_malloc: dining philosophers -----------
PRELOAD   = $(BINDIR)/libft_preload.so
PHILO_DIR = tests/philosopher/philo
PHILO_BIN = $(PHILO_DIR)/bin/philo
# n_philo time_to_die time_to_eat time_to_sleep [must_eat] — finite (must_eat).
PHILO_ARGS = 5 800 200 200 10

$(PRELOAD): tests/interpose.c $(BINDIR)/$(NAME)
	@mkdir -p $(BINDIR)
	@$(CC) -O2 -fPIC -shared $(INCLUDES) -o $@ tests/interpose.c \
		-L$(BINDIR) -lft_malloc $(RPATH)
	@echo "$(GREEN)✓ $@ (libc malloc -> ft_malloc shim) created$(RESET)"

philo-build:
	@$(MAKE) -s -C $(PHILO_DIR)

philo-ft: $(PRELOAD) philo-build
	@echo "$(GREEN)== dining philosophers on ft_malloc (LD_PRELOAD) ==$(RESET)"
	@LD_PRELOAD=$(CURDIR)/$(PRELOAD) $(PHILO_BIN) $(PHILO_ARGS); \
		echo "philo (ft_malloc) exit=$$?"

philo-libc: philo-build
	@echo "$(GREEN)== dining philosophers on libc malloc ==$(RESET)"
	@$(PHILO_BIN) $(PHILO_ARGS); echo "philo (libc) exit=$$?"

philo-helgrind: $(PRELOAD) philo-build
	@echo "$(GREEN)== philosophers on ft_malloc under Helgrind ==$(RESET)"
	@LD_PRELOAD=$(CURDIR)/$(PRELOAD) valgrind --tool=helgrind -q \
		--error-exitcode=99 $(PHILO_BIN) 4 1000 200 200 3 \
		&& echo "$(GREEN)✓ philosophers on ft_malloc: race-free$(RESET)"

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR) $(DBG_OBJ_DIR)

fclean: clean
	@echo "$(RED)Removing $(BINDIR)/$(NAME) and $(TESTBIN)...$(RESET)"
	@rm -f $(BINDIR)/$(NAME) $(TESTBIN)
	@rm -f $(BINDIR)/$(DBG_NAME) $(LEAK_BIN) $(ASAN_BIN) $(SYSCOUNT)
	@rm -f $(BENCH_BIN) $(BENCH_FULL) $(BENCH_MT_BIN) $(MT_BIN)
	@rm -f $(GNL_FT) $(GNL_LIBC) $(GNL_MT_FT) $(GNL_MT_LIBC) $(PRELOAD)
	@rm -f $(POOL_FT) $(POOL_LIBC)
	@rm -f $(BINDIR)/gnl_ft.o $(BINDIR)/gnl_libc.o \
		$(BINDIR)/gnl_mt_ft.o $(BINDIR)/gnl_mt_libc.o

re: fclean all

.PHONY: all clean fclean re debug leakcheck leakcheck-asan \
	leakcheck-valgrind leakcheck-all bench bench-shipped bench-mt \
	mt-stress helgrind bench-gnl bench-gnl-mt bench-pool bench-all \
	philo-build philo-ft philo-libc philo-helgrind


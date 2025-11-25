# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    by dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/01 20:00:47 by dlesieur          #+#    #+#              #
#    Updated: 2025/11/02 14:30:00 by dlesieur         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft_malloc.so
BINDIR = bin
TESTSRC = tests/get_next_line.c
TESTBIN = $(BINDIR)/gnl

CC = gcc
CFLAGS = -Wall -Wextra -Werror -fPIC -O2
INCLUDES = -I./include
LDFLAGS = -shared

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
            $(PRIVATE_DIR)/free_utils.c\
            $(PRIVATE_DIR)/free_utils2.c\
            $(PRIVATE_DIR)/malloc_helpers.c\
            $(PRIVATE_DIR)/malloc_triggers.c\
            $(PRIVATE_DIR)/internal_calloc.c\
            $(PRIVATE_DIR)/internal_free.c\
            $(PRIVATE_DIR)/internal_malloc.c\
            $(PRIVATE_DIR)/internal_realloc.c\
            $(PRIVATE_DIR)/internal_valloc.c\
            $(PRIVATE_DIR)/internals.c

HELPERS_SRCS = $(HELPERS_DIR)/accessors.c \
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
                $(HELPERS_DIR)/malloc_unblock_signals.c

DEBUG_SRCS = $(DEBUG_DIR)/stats.c \
                $(DEBUG_DIR)/helper.c \
                $(DEBUG_DIR)/show_alloc.c \
                $(DEBUG_DIR)/stat_utils.c \
                $(DEBUG_DIR)/stat_utils2.c \
                $(DEBUG_DIR)/stat_utils3.c \
                $(DEBUG_DIR)/stat_utils4.c \
                $(DEBUG_DIR)/stats.c \
                $(DEBUG_DIR)/table.c \
                $(DEBUG_DIR)/trace.c \
                $(DEBUG_DIR)/watch.c

UTILS_SRCS = $(UTILS_DIR)/powerof2.c \
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

all: $(BINDIR)/$(NAME) $(TESTBIN) $(MODE_FT_BIN) $(MODE_LIBC_BIN)

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

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Removing $(BINDIR)/$(NAME) and $(TESTBIN)...$(RESET)"
	@rm -f $(BINDIR)/$(NAME) $(TESTBIN)

re: fclean all

.PHONY: all clean fclean re


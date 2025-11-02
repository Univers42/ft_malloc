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
TESTSRC = tests/main/main.c
TESTBIN = $(BINDIR)/alloc

CC = gcc
CFLAGS = -Wall -Wextra -Werror -fPIC -O2
INCLUDES = -I./include
LDFLAGS = -shared

# Directories
SRC_DIR = src
OBJ_DIR = obj
CORE_DIR = $(SRC_DIR)/core
HELPERS_DIR = $(SRC_DIR)/helpers
DEBUG_DIR = $(SRC_DIR)/debug
UTILS_DIR = $(SRC_DIR)/utils

# Source files
CORE_SRCS = $(CORE_DIR)/malloc.c \
            $(CORE_DIR)/free.c \
            $(CORE_DIR)/realloc.c \
            $(CORE_DIR)/internals.c \
            $(CORE_DIR)/suite_alloc.c \
            $(CORE_DIR)/ft_mremap.c

HELPERS_SRCS = $(HELPERS_DIR)/accessors.c \
               $(HELPERS_DIR)/bcoalesce.c \
               $(HELPERS_DIR)/bsplit.c \
               $(HELPERS_DIR)/botch.c \
               $(HELPERS_DIR)/buckets.c \
               $(HELPERS_DIR)/chain.c \
               $(HELPERS_DIR)/dyn_core.c \
               $(HELPERS_DIR)/imalloc.c \
               $(HELPERS_DIR)/init.c \
               $(HELPERS_DIR)/pagealign.c \
               $(HELPERS_DIR)/signals.c

DEBUG_SRCS = $(DEBUG_DIR)/stats.c \
             $(DEBUG_DIR)/helper.c

UTILS_SRCS = $(UTILS_DIR)/powerof2.c

SRCS = $(CORE_SRCS) $(HELPERS_SRCS) $(DEBUG_SRCS) $(UTILS_SRCS)

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Colors
GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

all: $(BINDIR)/$(NAME) $(TESTBIN)

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
	@$(CC) $(CFLAGS) -I./include -o $(TESTBIN) $(TESTSRC) -L$(BINDIR) -lft_malloc -Wl,-rpath,'$$ORIGIN'

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Removing $(BINDIR)/$(NAME) and $(TESTBIN)...$(RESET)"
	@rm -f $(BINDIR)/$(NAME) $(TESTBIN)

re: fclean all

.PHONY: all clean fclean re


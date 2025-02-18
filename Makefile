# 编译器设置
CC = gcc
TOPDIR = $(shell pwd)

# 目录设置
OBJDIR = $(TOPDIR)/obj/
SRCDIR = $(TOPDIR)/src/
INCDIR = $(TOPDIR)/include/
BINDIR = $(TOPDIR)/bin/

# 目标文件
BIN = $(BINDIR)istatus

# 编译选项
WARNINGS = -Wall -Wextra -Werror
OPTIMIZE = -O2
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = $(OPTIMIZE)

# 基础编译选项
BASE_CFLAGS = -std=c11 -I$(INCDIR)

# 链接选项
LDFLAGS = -lpaho-mqtt3c

# 根据编译模式设置不同的编译选项
ifeq ($(DEBUG),1)
    CFLAGS = $(BASE_CFLAGS) $(DEBUG_FLAGS) $(WARNINGS)
else
    CFLAGS = $(BASE_CFLAGS) $(RELEASE_FLAGS) $(WARNINGS)
endif

# 源文件和目标文件
SRCS = $(wildcard $(SRCDIR)*.c)
OBJS = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SRCS))

# 默认目标
.PHONY: all
all: prepare $(BIN)

# 创建必要的目录
.PHONY: prepare
prepare:
	@mkdir -p $(OBJDIR) $(BINDIR)

# 编译可执行文件
$(BIN): $(OBJS)
	@echo "Linking $@"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

# 编译源文件
$(OBJDIR)%.o: $(SRCDIR)%.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# 安装
.PHONY: install
install: $(BIN)
	@echo "Installing to /usr/local/bin"
	@install -m 755 $(BIN) /usr/local/bin

# 清理
.PHONY: clean
clean:
	@echo "Cleaning build files"
	@rm -rf $(OBJDIR) $(BINDIR)

# 帮助信息
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Build options:"
	@echo "  make DEBUG=1  - Build with debug information"
	@echo "  make         - Build release version"

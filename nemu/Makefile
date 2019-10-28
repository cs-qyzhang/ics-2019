NAME = nemu

ifneq ($(MAKECMDGOALS),clean) # ignore check for make clean
ISA ?= x86
ISAS = $(shell ls src/isa/)
$(info Building $(ISA)-$(NAME))

ifeq ($(filter $(ISAS), $(ISA)), ) # ISA must be valid
$(error Invalid ISA. Supported: $(ISAS))
endif
endif

INC_DIR += ./include ./src/isa/$(ISA)/include
BUILD_DIR ?= ./build

ifdef SHARE
SO = -so
SO_CFLAGS = -fPIC -D_SHARE=1
SO_LDLAGS = -shared -fPIC
endif

OBJ_DIR ?= $(BUILD_DIR)/obj-$(ISA)$(SO)
BINARY ?= $(BUILD_DIR)/$(ISA)-$(NAME)$(SO)

include Makefile.git

.DEFAULT_GOAL = app

# Compilation flags
CC = gcc
LD = gcc
INCLUDES  = $(addprefix -I, $(INC_DIR))
CFLAGS   += -O2 -MMD -Wall -Werror -ggdb3 $(INCLUDES) -D__ISA__=$(ISA) -fomit-frame-pointer

QEMU_DIFF_PATH = $(NEMU_HOME)/tools/qemu-diff
QEMU_SO = $(QEMU_DIFF_PATH)/build/$(ISA)-qemu-so

$(QEMU_SO):
	$(MAKE) -C $(QEMU_DIFF_PATH)

# Files to be compiled
SRCS = $(shell find src/ -name "*.c" | grep -v "isa")
SRCS += $(shell find src/isa/$(ISA) -name "*.c")
OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)
EXPR_SRC_DIR = src/monitor/debug/expr
EXPR_OBJ_DIR = $(OBJ_DIR)/monitor/debug
OBJS += $(EXPR_OBJ_DIR)/parser.o
OBJS += $(EXPR_OBJ_DIR)/lex.o

# Compilation patterns
$(OBJ_DIR)/%.o: src/%.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(SO_CFLAGS) -c -o $@ $<

# bison
$(EXPR_OBJ_DIR)/parser.o: $(EXPR_SRC_DIR)/parser.y $(EXPR_SRC_DIR)/def.h
	@echo + BISION $(EXPR_SRC_DIR)/parser.y
	@mkdir -p $(dir $@)
	@bison -d -v -o $(EXPR_OBJ_DIR)/parser.tab.c --defines=$(EXPR_OBJ_DIR)/parser.tab.h $(EXPR_SRC_DIR)/parser.y 
	@echo + CC $(EXPR_OBJ_DIR)/parser.tab.c
	@$(CC) $(CFLAGS) $(SO_CFLAGS) -c -o $(EXPR_OBJ_DIR)/parser.o $(EXPR_OBJ_DIR)/parser.tab.c -Lfl -Ly -I$(EXPR_SRC_DIR)

# flex
$(EXPR_OBJ_DIR)/lex.o: $(EXPR_OBJ_DIR)/parser.o $(EXPR_SRC_DIR)/lex.l $(EXPR_SRC_DIR)/def.h
	@echo + FLEX $(EXPR_SRC_DIR)/lex.l
	@mkdir -p $(dir $@)
	@flex -o $(EXPR_OBJ_DIR)/lex.ll.c $(EXPR_SRC_DIR)/lex.l
	@echo + CC $(EXPR_OBJ_DIR)/lex.ll.c
	@$(CC) $(CFLAGS) -Wno-unused-function $(SO_CFLAGS) -c -o $(EXPR_OBJ_DIR)/lex.o $(EXPR_OBJ_DIR)/lex.ll.c -Lfl -Ly -I$(EXPR_SRC_DIR)

# Depencies
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app run gdb clean run-env $(QEMU_SO)
app: $(BINARY)

override ARGS ?= -l $(BUILD_DIR)/nemu-log.txt
override ARGS += -d $(QEMU_SO)

# Command to execute NEMU
IMG :=
NEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

$(BINARY): $(OBJS)
	$(call git_commit, "compile")
	@echo + LD $@
	@$(LD) -O2 -rdynamic $(SO_LDLAGS) -o $@ $^ -lSDL2 -lreadline -ldl
#	@ln -s $(BINARY) $(ISA)-$(NAME)$(SO)

run-env: $(BINARY) $(QEMU_SO)

run: run-env
	$(call git_commit, "run")
	$(NEMU_EXEC)

gdb: run-env
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

clean:
	-rm -rf $(BUILD_DIR)
	$(MAKE) -C tools/gen-expr clean
	$(MAKE) -C tools/qemu-diff clean

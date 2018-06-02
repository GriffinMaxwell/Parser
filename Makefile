# Commands
CC=gcc
CPP=g++
MKDIR_P ?= mkdir -p

# Directories
BUILD_DIR := build
SRC_DIRS := \
	source \
	source/util

# File lists
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Compiler parameters
CC_INCL_DIRS := $(SRC_DIRS:%=-I%)

# Rules
all: $(OBJS)
	@echo "Linking objects..."
	@$(CC) $^

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

$(BUILD_DIR)/%.d: %.c
	@$(MKDIR_P) $(dir $@)
	@$(CPP) $< $(CC_INCL_DIRS) -MM -MT $(@:.d=.o) >$@

$(BUILD_DIR)/%.o: %.c
	@$(MKDIR_P) $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CC_INCL_DIRS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)/*/
	@echo "Build directory is clean."

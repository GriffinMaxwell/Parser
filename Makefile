# Commands
CC=gcc
CPP=g++
MKDIR_P ?= mkdir -p

# Directories
BUILD_DIR := build
SRC_DIRS := \
	source

# File lists
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Rules
all: $(OBJS)
	$(CC) $^

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

$(BUILD_DIR)/%.d: %.c
	@$(MKDIR_P) $(dir $@)
	@$(CPP) $< -MM -MT $(@:.d=.o) >$@

$(BUILD_DIR)/%.o: %.c
	@$(MKDIR_P) $(dir $@)
	@echo "Compiling $<..."
	@$(CC) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)/*/
	@echo "Build directory is clean."

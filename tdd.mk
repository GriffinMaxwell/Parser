#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

####################
#--- Inputs
####################
# Directories used to search for include files.
INCLUDE_DIRS := \
	source \
	source/util

# Directories containing source files to build into the library
SRC_DIRS := \
	source/util

# Specific source files to build into library. Helpful when not all code in a directory can be built for test (hopefully a temporary situation)
SRC_FILES := \
	source/Lexer_StaticLookup.c

# Directories containing unit test code build into the unit test runner
TEST_SRC_DIRS := \
	test

# Specific source files to build into the unit test runner
TEST_SRC_FILES := \

# Directories containing mock source files to build into the test runner
MOCKS_SRC_DIRS := \
	test/util

####################
#--- How and Where
####################
# The name of the thing being built
COMPONENT_NAME = Parser

# Name of the test executable
TEST_TARGET = test/$(COMPONENT_NAME)_test_target

# Where CppUTest home dir found
ifeq ($(OS),Windows_NT)	## TODO: Super hacky for now
	CPPUTEST_HOME = C:/Users/griff/git/cpputest
else
	CPPUTEST_HOME = /usr/local/Cellar/cpputest/3.8
endif

# a directory where o and d files go
CPPUTEST_OBJS_DIR := test/build

# a directory where libs go
CPPUTEST_LIB_DIR := test/build

CPPUTEST_USE_EXTENSIONS = Y
CPPUTEST_WARNINGFLAGS =
CPPUTEST_PEDANTIC_ERRORS = N

# Flags
CPPUTEST_CXXFLAGS += \
	-I$(CPPUTEST_HOME)/include \
	-I$(CPPUTEST_HOME)/include/CppUTest \
	-I$(CPPUTEST_HOME)/include/CppUTestExt \

# Silence all warnings (because they are annoying)
CPPUTEST_CPPFLAGS += -w -fpermissive
CPPUTEST_FLAGS += -w

CXXFLAGS += \
	-include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

CFLAGS += \
	-include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h \

LD_LIBRARIES += -L$(CPPUTEST_HOME) -lCppUTest -lCppUTestExt -lpthread

include test/MakefileWorker.mk

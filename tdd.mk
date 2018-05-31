#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

####################
#--- Inputs
####################
# Directories used to search for include files.
INCLUDE_DIRS := \

# Directories containing source files to build into the library
SRC_DIRS := \

# Specific source files to build into library. Helpful when not all code in a directory can be built for test (hopefully a temporary situation)
SRC_FILES := \

# Directories containing unit test code build into the unit test runner
TEST_SRC_DIRS := \
	test

# Specific source files to build into the unit test runner
TEST_SRC_FILES := \

# Directories containing mock source files to build into the test runner
MOCKS_SRC_DIRS := \

####################
#--- How and Where
####################
# The name of the thing being built
COMPONENT_NAME = Parser

# Name of the test executable
TEST_TARGET = test/$(COMPONENT_NAME)_tests

# Where CppUTest home dir found
CPPUTEST_HOME = C:/Users/griff/git/cpputest

# a directory where o and d files go
CPPUTEST_OBJS_DIR := test/build/obj

# a directory where libs go
CPPUTEST_LIB_DIR := test/build/lib

CPPUTEST_USE_EXTENSIONS = Y

# Flags
CPPUTEST_CXXFLAGS += \
	-I$(CPPUTEST_HOME)/include/CppUTest \
	-I$(CPPUTEST_HOME)/include/CppUTestExt \

CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h

LD_LIBRARIES += -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt -lpthread

include test/MakefileWorker.mk

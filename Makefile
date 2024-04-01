NAME := fs_cli
FNAME := filesystem_cli

CC := gcc
CXX := g++
FC := gfortran

BUILD_DIR := ./build

INC := -Iinclude/
CXXFLAGS := -std=c++20 -Wall $(INC)
CFLAGS := -Wall $(INC)
FFLAGS := -Wall

cdir = src/common/
SRCS = $(cdir)common.c $(cdir)filesystem.cpp $(cdir)dynamic.cpp $(cdir)inquire.cpp $(cdir)pure.cpp $(cdir)platform.cpp $(cdir)resolve.cpp $(cdir)symlink.cpp $(cdir)windows.cpp
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

fdir = $(cdir)fortran/
FSRCS = $(fdir)filesystem.f90
FOBJS := $(FSRCS:%=$(BUILD_DIR)/%.o)

all: $(NAME)

$(NAME): app/main.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $< $(LDFLAGS)

$(FNAME): app/fortran/main.f90 $(FOBJS) $(OBJS)
	$(FC) $(FFLAGS) $(FOBJS) $(OBJS) -o $@ $< $(LDFLAGS) -lstdc++

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.f90.o: %.f90
	mkdir -p $(dir $@)
	$(FC) $(FFLAGS) -c $< -o $@

.PHONY: all

clean:
	$(RM) -r $(BUILD_DIR)

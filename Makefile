NAME := fs_cli
FNAME := filesystem_cli

CC := gcc
CXX := g++
FC := gfortran

BUILD_DIR := build

INC := -Iinclude/
CXXFLAGS := -std=c++20 -Wall $(INC)
CFLAGS := -Wall $(INC)
FFLAGS := -Wall

cdir = src/common/
COMM_SRCS = $(cdir)common.c $(cdir)dynamic.c $(cdir)partition.c
SRCS = $(cdir)filesystem.cpp $(cdir)c_ifc.cpp $(cdir)dynamic.cpp $(cdir)inquire.cpp $(cdir)pure.cpp $(cdir)platform.cpp $(cdir)resolve.cpp $(cdir)symlink.cpp $(cdir)windows.cpp
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o) $(COMM_SRCS:%=$(BUILD_DIR)/%.o)

fdir = $(cdir)fortran/
FSRCS = $(fdir)filesystem.F90
FOBJS := $(FSRCS:%=$(BUILD_DIR)/%.o)

fbd = $(BUILD_DIR)/$(fdir)

main = $(BUILD_DIR)/$(NAME)

main_f = $(BUILD_DIR)/filesystem_cli

ifeq ($(OS),Windows_NT)
	SHELL := pwsh.exe
	.SHELLFLAGS := -Command
	LDFLAGS := -lUserenv
	RM := Remove-Item -Recurse
	COMMENT = ".SHELLFLAGS -Command needed to get Make to use powershell rather than cmd"
else
	RM := rm -rf
endif

MKDIR := mkdir -p

.PHONY: $(main)

all: mbd $(main) $(main_f)

mbd: $(fbd)

$(fbd):
	$(MKDIR) $(fbd)


$(main): app/main.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $< $(LDFLAGS)

$(main_f): app/fortran/main.f90 $(FOBJS) $(OBJS)
	$(FC) $(FFLAGS) $(FOBJS) $(OBJS) -o $@ $< $(LDFLAGS) -lstdc++

$(FNAME): app/fortran/main.f90 $(FOBJS) $(OBJS)
	$(FC) $(FFLAGS) $(FOBJS) $(OBJS) -o $@ $< $(LDFLAGS) -lstdc++

$(BUILD_DIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.f90.o: %.f90
	$(FC) $(FFLAGS) -c $< -o $@

$(BUILD_DIR)/%.F90.o: %.F90
	$(FC) $(FFLAGS) -c $< -o $@

clean:
	$(RM) $(BUILD_DIR)

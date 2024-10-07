NAME := fs_cli
FNAME := filesystem_cli
LIB := libfilesystem.a

ifeq ($(origin CC),default)
CC = gcc
endif

ifeq ($(origin CXX),default)
CXX = g++
endif

FC := gfortran

# Clang, including AppleClang:
# make CC=clang CXX=clang++

# NVHPC:
# make CC=nvc CXX=nvc++ FC=nvfortran

# Intel oneAPI
# make CC=icx CXX=icpx FC=ifx

BUILD_DIR := build

INC := -Iinclude/

# optional, but useful
cfeat = -DHAVE_MERSENNE_TWISTER -DHAVE_GETLOADAVG
cppfeat = -DHAVE_MERSENNE_TWISTER

CXXFLAGS := -std=c++20 -O3 -DNDEBUG $(cppfeat) $(INC)
CFLAGS := -O3 -DNDEBUG $(cfeat) $(INC)
FFLAGS := -O3 -DNDEBUG

ARFLAGS := rcs

cdir = src/common/
fdir = $(cdir)fortran/

COMM_SRCS = $(cdir)common.c $(cdir)compiler.c $(cdir)cpu.c $(cdir)cygwin.c $(cdir)exepath.c $(cdir)env.c $(cdir)home.c $(cdir)libpath.c $(cdir)limits.c $(cdir)mkdtemp.c $(cdir)owner.c $(cdir)os.c $(cdir)partition.c $(cdir)realpath.c $(cdir)sysctl.c $(cdir)touch.c $(cdir)uid.c $(cdir)uname.c $(cdir)which.c $(cdir)winsock.c $(cdir)windows.c
SRCS = $(cdir)filesystem.cpp $(cdir)c_ifc.cpp $(cdir)copy.cpp $(cdir)ifc.cpp $(cdir)inquire.cpp $(cdir)mkdir.cpp $(cdir)mkdtemp.cpp $(cdir)pure.cpp $(cdir)platform.cpp $(cdir)resolve.cpp $(cdir)space.cpp $(cdir)symlink.cpp $(cdir)time.cpp
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o) $(COMM_SRCS:%=$(BUILD_DIR)/%.o)

fbd = $(BUILD_DIR)/$(fdir)

lib = $(BUILD_DIR)/$(LIB)
main = $(BUILD_DIR)/$(NAME)

ifeq ($(OS),Windows_NT)
	SHELL := pwsh.exe
	.SHELLFLAGS := -Command
	LDFLAGS := -lws2_32 -lOle32 -lShell32 -luuid -lUserenv
	RM := Remove-Item -Recurse
	COMMENT = ".SHELLFLAGS -Command needed to get Make to use powershell rather than cmd"
else
	RM := rm -rf
endif

ifeq (icpx,$(findstring icpx,$(CXX)))
  CXXLIBS := -lstdc++
else ifeq (nvc++, $(findstring nvc++,$(CXX)))
  CXXLIBS := -lstdc++
else ifeq (clang++,$(findstring clang++,$(CXX)))
  CXXLIBS := -lc++
else ifeq  (g++,$(findstring g++,$(CXX)))
  CXXLIBS := -lstdc++
endif

ifeq (gfortran,$(findstring gfortran,$(FC)))
  FFLAGS += -J$(BUILD_DIR)
endif

MKDIR := mkdir -p

.PHONY: $(main)

all: mbd $(lib) $(main) $(main_f)

mbd: $(fbd)

$(fbd):
	$(MKDIR) $(fbd)

$(lib): $(OBJS) $(FOBJS)
	$(AR) $(ARFLAGS) $@ $?

$(main): app/main.cpp $(lib)
	$(CXX) $(CXXFLAGS) $(lib) -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Fortran
ifdef fortran
main_f = $(BUILD_DIR)/$(FNAME)

FSRCS = $(fdir)filesystem.F90
FOBJS := $(FSRCS:%=$(BUILD_DIR)/%.o)

$(main_f): app/fortran/main.f90 $(FOBJS) $(lib)
	$(FC) $(FFLAGS) $(FOBJS) $(lib) -o $@ $< $(LDFLAGS) $(CXXLIBS)

$(BUILD_DIR)/%.f90.o: %.f90
	$(FC) $(FFLAGS) -c $< -o $@

$(BUILD_DIR)/%.F90.o: %.F90
	$(FC) $(FFLAGS) -c $< -o $@
endif

clean:
	$(RM) $(BUILD_DIR)

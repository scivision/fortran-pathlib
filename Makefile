NAME := fs_cli
FNAME := filesystem_cli
LIB := libfilesystem.a

cpp := 1

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

comdir = src/common/
cdir = src/c/
fdir = $(comdir)fortran/

COMM_SRCS = $(comdir)c.cpp $(comdir)common.c $(comdir)compiler.cpp $(comdir)cpu.c $(comdir)cygwin.c $(comdir)exepath.cpp $(comdir)env.cpp $(comdir).cpp $(comdir)libpath.cpp $(comdir)limits.cpp $(comdir)mkdtemp.c $(comdir)owner.c $(comdir)os.c $(comdir)partition.c $(comdir)realpath.c $(comdir)sysctl.cpp $(comdir)touch.cpp $(comdir)uid.cpp $(comdir)uname.cpp $(comdir)which.c $(comdir)windows.cpp $(comdir)winsock.cpp

ifeq (cpp,1)
SRCS = $(comdir)filesystem.cpp $(comdir)c_ifc.cpp $(comdir)copy.cpp $(comdir)ifc.cpp $(comdir)inquire.cpp $(comdir)mkdir.cpp $(comdir)mkdtemp.cpp $(comdir)pure.cpp $(comdir)platform.cpp $(comdir)resolve.cpp $(comdir)space.cpp $(comdir)symlink.cpp $(comdir)time.cpp
else
SRCS = $(cdir)filesystem.c $(cdir)copy.c $(cdir)inquire.c $(cdir)mkdir.c $(cdir)mkdtemp.c $(cdir)pure.c $(cdir)platform.c $(cdir)resolve.c $(cdir)space.c $(cdir)symlink.c $(cdir)time.c
endif

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

# cosmocc wants OBJS instead of $(lib). The latter is OK for standard compilers.
$(main): app/main.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $< $(LDFLAGS)

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

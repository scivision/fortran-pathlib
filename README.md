# Ffilesystem: for Fortran using C++ filesystem or standalone C stdlib functions

[![DOI](https://zenodo.org/badge/433875623.svg)](https://zenodo.org/badge/latestdoi/433875623)
[![ci](https://github.com/scivision/fortran-filesystem/actions/workflows/ci.yml/badge.svg)](https://github.com/scivision/fortran-filesystem/actions/workflows/ci.yml)
[![ci_windows](https://github.com/scivision/fortran-filesystem/actions/workflows/ci_windows.yml/badge.svg)](https://github.com/scivision/fortran-filesystem/actions/workflows/ci_windows.yml)
[![oneapi-linux](https://github.com/scivision/fortran-filesystem/actions/workflows/oneapi-linux.yml/badge.svg)](https://github.com/scivision/fortran-filesystem/actions/workflows/oneapi-linux.yml)
[![ci_fpm](https://github.com/scivision/fortran-filesystem/actions/workflows/ci_fpm.yml/badge.svg)](https://github.com/scivision/fortran-filesystem/actions/workflows/ci_fpm.yml)
[![ci_meson](https://github.com/scivision/fortran-filesystem/actions/workflows/ci_meson.yml/badge.svg)](https://github.com/scivision/fortran-filesystem/actions/workflows/ci_meson.yml)

Platform independent (Linux, macOS, Windows, Cygwin, WSL, BSD, ...), object-oriented Fortran filesystem "Ffilesystem" path manipulation library.
Ffilesystem header
[ffilesystem.h](./include/ffilesystem.h)
can be used from C and C++ project code--see
[example](./example).

For C++ users, **C++17 standard** is required.
For C users, **C99 standard** is required.
Fortran uses **Fortran 2003 standard** and either the C or C++ Ffilesystem backend.

For full features, Ffilesystem uses
[C++ stdlib filesystem](https://en.cppreference.com/w/cpp/filesystem).
If a compatible C++ compiler isn't available, Ffilesystem downloads and uses
[CWalk](https://github.com/likle/cwalk)
and C runtime library.

Inspired by (and benchmarked against)
[Python pathlib](https://docs.python.org/3/library/pathlib.html).

Important Ffilesystem functions are [benchmarked](./test/bench/bench.md) to help improve performance.

Advanced / conceptual development takes place in [ffilesystem-concepts](https://github.com/scivision/ffilesystem-concepts) repo.

## Compiler support

Ffilesystem supports compilers including:

* GCC &ge; 8 (gcc/g++, gfortran)
    * GCC 7.x works with `cmake -Dffilesystem_cpp=off`
* LLVM Clang &ge; 9 (clang/clang++, flang or gfortran)
* Intel oneAPI (icx, icpx, ifx)
* AMD AOCC (clang/clang++, flang)
* NVidia HPC SDK (nvc++, nvfortran)
* Visual Studio (C/C++)
* Cray: using Cray compilers alone (cc, CC, ftn) or using GCC or Intel backend

C++ interface requires compiler to support `<filesystem>`.
The older `<experimental/filesystem>` is NOT supported, because it is missing vital lexical operations fundamental to many filesystem operations.
To use such compilers, disable C++ support:

```sh
cmake -Bbuild -Dffilesystem_cpp=off
```

For other/older compilers that don't have C++ `<filesystem>` try `cmake -Dffilesystem_cpp=off`.

### libstdc++

Some systems have broken, obsolete, or incompatible libstdc++.

**Intel**: If Intel compiler linker errors use GCC >= 9.1.
This can be done by setting environment variable CXXFLAGS to the top level GCC >= 9.1 directory.
Set environment variable CXXFLAGS for
[Intel GCC toolchain](https://www.intel.com/content/www/us/en/develop/documentation/oneapi-dpcpp-cpp-compiler-dev-guide-and-reference/top/compiler-reference/compiler-options/compiler-option-details/compatibility-options/gcc-toolchain.html)
like:

```sh
export CXXFLAGS=--gcc-toolchain=/opt/rh/gcc-toolset-10/root/usr/
```

which can be determined like:

```sh
scl enable gcc-toolset-10 "which g++"
```

## Build

Ffilesystem can be built with CMake or Fortran Package Manager (FPM).

"libffilesystem.a" is the library binary built that contains the Fortran "filesystem" module--it is the only binary you need to use in your project.

Please see the [API docs](./API.md) for extensive list of functions/subroutines.

Use any one of these methods to build Ffilesystem (CMake is recommended):

CMake:

```sh
cmake -B build
cmake --build build
# optional
ctest --test-dir build
```

Meson:

```sh
meson setup build
meson compile -C build
# optional
meson test -C build
```

Fortran Package Manager:

```sh
FPM_CXXFLAGS=-std=c++20 fpm build
```

GNU Make:

```sh
make
```

We provide Fortran REPL "filesystem_cli" and C++ REPL "fs_cli" for interactive testing of Ffilesystem routines.

### Build options

Fortran "filesystem" module contains OPTIONAL (enabled by default) Fortran type "path_t" that contains properties and methods.
The "path_t" type uses getter and setter procedure to access the path as a string `character(:), allocatable`.

```fortran
use filesystem, only : path_t

type(path_t) :: p

p = path_t("my/path")  !< setter

print *, "path: ", p%path() !< getter
```

The CMake and Meson scripts detect if Fortran 2003 `type` is available and enable `path_t` by default.
To manually enable / disable `path_t` with CMake set command option `cmake -DHAVE_F03TYPE=1` or `cmake -DHAVE_F03TYPE=0` respectively.

## Usage from other projects

The [example](./example) directory contains a use pattern from external projects.
One can either `cmake --install build` ffilesystem or use CMake ExternalProject or
[FetchContent](https://gist.github.com/scivision/245a1f32498d15a87a507051857327d9)
from the other project.
To find ffilesystem in your CMake project:

```cmake
find_package(ffilesystem CONFIG REQUIRED)
```

Note the CMake package variables `ffilesystem_cpp` and `ffilesystem_fortran` indicate whether ffilesystem was built with C++ and/or Fortran support.

[ffilesystem.cmake](./cmake/ffilesystem.cmake) would be included from the other project to find or build Ffilesystem automatically.
It provides the appropriate imported targets for shared or static builds, including Windows DLL handling.

## Notes

A few topics on unsupported features:

### non-ASCII characters

Due to compiler limitations, currently Ffilesystem only officially supports ASCII characters.

The UCS
[selected_char_kind('ISO_10646')](https://gcc.gnu.org/onlinedocs/gfortran/SELECTED_005fCHAR_005fKIND.html),
is an *optional* feature of Fortran 2003 standard.
Intel oneAPI does not support `selected_char_kind('ISO_10646')` as of this writing.

filesystem currently uses the default Fortran `character` kind, which is ASCII.
This typically allows pass-through of UTF-8 characters, but this is not guaranteed.

### C++ filesystem discussion

Security
[research](https://www.reddit.com/r/cpp/comments/151cnlc/a_safety_culture_and_c_we_need_to_talk_about/?rdt=62365)
led to
[TOCTOU](https://en.wikipedia.org/wiki/Time-of-check_to_time-of-use)-related
patches to the C++ filesystem library in various C++ standard library implementations noted in that discussion.
Ffilesystem does NOT use remove_all, which was the TOCTOU concern addressed above.

Since the underlying C++ filesystem is not thread-safe, race conditions can occur if multiple threads are accessing the same filesystem object regardless of the code language used in the Ffilesystem library.

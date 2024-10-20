project('ffilesystem', ['c', 'cpp'],
version: '5.8.0',
default_options: ['c_std=c99', 'cpp_std=c++23,c++20,c++17', 'buildtype=release', 'b_ndebug=if-release'],
meson_version : '>=1.4.0')

use_cpp = get_option('cpp')
# mean to use C++17 <filesystem> library

have_filesystem = use_cpp ? '-DHAVE_CXX_FILESYSTEM' : []

fortran = get_option('fortran')
if fortran
  add_languages('fortran', native: false, required: true)
endif

cpp_std = get_option('cpp_std')
message('C++ standard: ' + cpp_std)

fs = import('fs')

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

if ['gcc', 'clang', 'intel-llvm'].contains(cc.get_id())
  add_project_arguments('-Werror=implicit-function-declaration', language: 'c')
endif

fsrcs = fortran ? files('src/common/fortran/filesystem.F90') : []

comm_src = files(
'src/common/c.cpp',
'src/common/common.c',
'src/common/absolute.cpp',
'src/common/compiler.cpp',
'src/common/copy.cpp',
'src/common/cpu.cpp',
'src/common/cygwin.cpp',
'src/common/exepath.cpp',
'src/common/env.cpp',
'src/common/filesystem.cpp',
'src/common/home.cpp',
'src/common/inquire.cpp',
'src/common/libpath.cpp',
'src/common/limits.cpp',
'src/common/mkdtemp.cpp',
'src/common/mkdir.cpp',
'src/common/owner.cpp',
'src/common/os.c',
'src/common/partition.cpp',
'src/common/platform.cpp',
'src/common/pure2.cpp',
'src/common/realpath.cpp',
'src/common/space.cpp',
'src/common/sysctl.cpp',
'src/common/symlink.cpp',
'src/common/time.cpp',
'src/common/touch.cpp',
'src/common/uid.cpp',
'src/common/uname.cpp',
'src/common/which.cpp',
'src/common/winsock.cpp',
'src/common/windows.cpp'
)

if use_cpp

cppsrcs = files(
'src/common/c_ifc.cpp',
'src/common/ifc.cpp',
'src/common/pure.cpp',
'src/common/resolve.cpp'
)
else

cppsrcs = []
comm_src += files(
'src/c/pure.c',
'src/c/resolve.c'
)
endif

inc = include_directories('include')

is_windows = host_machine.system() == 'windows'

# -- dladdr check
dladdr_def = []
dl = []

if not is_windows
  dl = cpp.find_library('dl', required: false)

  have_dladdr = cpp.links('''#define _GNU_SOURCE
  #include <dlfcn.h>
  static void dl_dummy_func() {}

  int main(){

  Dl_info info;
  if(!dladdr( (void*)&dl_dummy_func, &info))
    return 1;
  return 0;
  }
  ''', dependencies: dl, name: 'dladdr')
  if have_dladdr
    dladdr_def = '-DHAVE_DLADDR'
  endif
endif

# -- getloadavg check
getloadavg_def = []

if not is_windows
  have_getloadavg = cc.links('''
  #if defined(__linux__) && !defined(_DEFAULT_SOURCE)
  #define _DEFAULT_SOURCE
  #endif

  #include <stdlib.h>

  int main(void){
    double load[3] = {0.0, 0.0, 0.0};
    if (getloadavg(load, 3) < 0)
      return 1;
    return 0;
  }
  ''', name: 'getloadavg')
  if have_getloadavg
    getloadavg_def = '-DHAVE_GETLOADAVG'
  endif
endif

# -- C++ Feature checks
chrono_def = []
mkdtemp_def = []

fs_lib = []

if use_cpp

if cpp.get_id() == 'gcc' and cpp.version().version_compare('<9.1.0')
  fs_lib = [cpp.find_library('stdc++fs'), cpp.find_library('stdc++')]
endif

fs_check = '''
#include <filesystem>
static_assert(__cpp_lib_filesystem, "No C++ filesystem support");
int main () {
std::filesystem::path tgt(".");
auto h = tgt.has_filename();
return 0;
}'''

if not cpp.links(fs_check, dependencies: fs_lib, name: 'C++ filesystem')
  error('No C++ filesystem support')
endif

# -- chrono check

chrono_src = '''
#include <chrono>
#include <filesystem>

int main(){

std::filesystem::file_time_type t_fs;
auto t_sys = std::chrono::clock_cast<std::chrono::system_clock>(t_fs);
return 0;
}
'''
if cpp.links(chrono_src, dependencies: fs_lib, name: 'C++20 chrono')
  chrono_def = '-DHAVE_CLOCK_CAST'
endif

# -- Mersenne mkdtemp check

mkdtemp_src = '''
#include <random>

int main(){
  std::mt19937 mt_rand(0);
  return 0;
}
'''

if cpp.links(mkdtemp_src, name: 'C++ Mersenne Twister for mkdtemp()')
  mkdtemp_def = '-DHAVE_MERSENNE_TWISTER'
endif

# -- C++ map.contains
cpp_map_contains = cpp.compiles('''#include <map>
int main(){
std::map<int, char> m{{1, 'a'}, {2, 'b'}};
return m.contains(2);
}''', name: 'C++ map.contains')

endif # use_cpp

# -- Fortran feature checks
f03type_def = []

if fortran

fc = meson.get_compiler('fortran')

f03type_src = '''
program main
type :: path_t
character(:), allocatable :: s
end type
end program
'''

have_f03type = fc.compiles(f03type_src, name: 'Fortran 2003 derived type')
if not have_f03type
  f03type_def = '-DNO_F03TYPE'
endif

endif

# Windows-specific
libwin = []

if is_windows

libwin = [cc.find_library('Userenv'), cc.find_library('ws2_32')]

endif

# -- main library

filesystem = library('filesystem',
sources: [fsrcs, comm_src, cppsrcs],
include_directories: [inc],
install: true,
c_args: [],
cpp_args: [have_filesystem, chrono_def, dladdr_def, getloadavg_def, mkdtemp_def],
fortran_args: f03type_def,
dependencies: [libwin, fs_lib, dl],
)

if use_cpp and cpp_map_contains
  executable('fs_cli', sources: 'app/main.cpp', link_with: filesystem, include_directories: inc, install: true)
endif

if fortran

executable('filesystem_cli',
  sources: 'app/fortran/main.f90',
  link_with: filesystem,
  install: true)

endif

if not meson.is_subproject()
  foreach d : ['core', 'environment', 'executable', 'fortran', 'permissions']
    subdir('test/' + d)
  endforeach

  if use_cpp and cpp_map_contains
    executable('ffilesystem_bench', 'test/bench/ffilesystem_bench.cpp', link_with: filesystem, include_directories: inc, install: true)
  endif
endif

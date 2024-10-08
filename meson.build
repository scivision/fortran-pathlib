project('ffilesystem', ['c', 'cpp'],
version: '5.8.0',
default_options: ['c_std=c99', 'cpp_std=c++23,c++20,c++17', 'buildtype=release', 'b_ndebug=if-release'],
meson_version : '>=1.4.0')

use_cpp = get_option('cpp')
# mean to use C++17 <filesystem> library

fortran = get_option('fortran')
if fortran
  add_languages('fortran', native: false, required: true)
endif

cpp_std = get_option('cpp_std')
message('C++ standard: ' + cpp_std)

fs = import('fs')

cc = meson.get_compiler('c')

if ['gcc', 'clang', 'intel-llvm'].contains(cc.get_id())
  add_project_arguments('-Werror=implicit-function-declaration', language: 'c')
endif

fsrcs = fortran ? files('src/common/fortran/filesystem.F90') : []

comm_src = files('src/common/common.c',
'src/common/compiler.c',
'src/common/cpu.c',
'src/common/cygwin.c',
'src/common/exepath.c',
'src/common/env.c',
'src/common/home.c',
'src/common/mkdtemp.c',
'src/common/libpath.c',
'src/common/limits.c',
'src/common/owner.c',
'src/common/os.c',
'src/common/partition.c',
'src/common/realpath.c',
'src/common/sysctl.c',
'src/common/touch.c',
'src/common/uid.c',
'src/common/uname.c',
'src/common/which.c',
'src/common/winsock.c',
'src/common/windows.c'
)

if use_cpp

cwalk_dep = []
cwalk_inc = []

cppsrcs = files(
'src/common/copy.cpp',
'src/common/filesystem.cpp',
'src/common/c_ifc.cpp',
'src/common/ifc.cpp',
'src/common/inquire.cpp',
'src/common/mkdir.cpp',
'src/common/mkdtemp.cpp',
'src/common/pure.cpp',
'src/common/platform.cpp',
'src/common/resolve.cpp',
'src/common/space.cpp',
'src/common/symlink.cpp',
'src/common/time.cpp'
)
else

cwalk_proj = subproject('cwalk')
cwalk_dep = cwalk_proj.get_variable('cwalk')
cwalk_inc = cwalk_proj.get_variable('cwalk_inc')

cppsrcs = []
comm_src += files('src/c/copy.c',
'src/c/filesystem.c',
'src/c/inquire.c',
'src/c/mkdir.c',
'src/c/platform.c',
'src/c/pure.c',
'src/c/resolve.c',
'src/c/space.c',
'src/c/symlink.c',
'src/c/time.c')
endif

inc = include_directories('include')

is_windows = host_machine.system() == 'windows'

# -- dladdr check
dladdr_def = []
dl = []

if not is_windows
  dl = cc.find_library('dl', required: false)

  have_dladdr = cc.links('''#define _GNU_SOURCE
  #include <dlfcn.h>
  static void dl_dummy_func() {}

  int main(void){

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

cpp = meson.get_compiler('cpp')

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
win32_symlink_def = []

if is_windows

libwin = [cc.find_library('Userenv'), cc.find_library('ws2_32')]

if use_cpp
  symlink_run = cpp.run(fs.read('cmake/fs_check/check_fs_symlink.cpp'), dependencies: fs_lib, name: 'Symlink')
  if symlink_run.returncode() != 0
    win32_symlink_def = '-DWIN32_SYMLINK'
  endif
endif

endif

# -- main library

filesystem = library('filesystem',
sources: [fsrcs, comm_src, cppsrcs],
include_directories: [cwalk_inc, inc],
install: true,
c_args: [mkdtemp_def, dladdr_def, getloadavg_def],
cpp_args: [chrono_def, mkdtemp_def, win32_symlink_def],
fortran_args: f03type_def,
dependencies: [libwin, fs_lib, dl],
link_with: cwalk_dep
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
  foreach d : [
      'canonical_resolve', 'core', 'environment', 'executable', 'expanduser',
      'fortran', 'permissions', 'relative'
      ]
    subdir('test/' + d)
  endforeach

  if use_cpp and cpp_map_contains
    executable('ffilesystem_bench', 'test/bench/ffilesystem_bench.cpp', link_with: filesystem, include_directories: inc, install: true)
  endif
endif

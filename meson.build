project('ffilesystem', ['c', 'cpp'],
version: '6.0.0',
default_options: ['c_std=c99', 'cpp_std=c++23,c++20,c++17', 'buildtype=release', 'b_ndebug=if-release'],
meson_version : '>=1.4.0')

use_cpp = get_option('cpp')
# mean to use C++17 <filesystem> library

trace = get_option('trace')
trace_def = trace ? '-DFS_TRACE=1' : []

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

fsrcs = fortran ? files('src/fortran/filesystem.F90') : []

comm_src = files(
'src/c.cpp',
'src/absolute.cpp',
'src/compiler.cpp',
'src/copy.cpp',
'src/cpu.cpp',
'src/cygwin.cpp',
'src/exepath.cpp',
'src/env.cpp',
'src/filesystem.cpp',
'src/home.cpp',
'src/inquire.cpp',
'src/libpath.cpp',
'src/limits.cpp',
'src/locale.cpp',
'src/mkdtemp.cpp',
'src/mkdir.cpp',
'src/owner.cpp',
'src/os.c',
'src/partition.cpp',
'src/platform.cpp',
'src/pure.cpp',
'src/realpath.cpp',
'src/resolve.cpp',
'src/space.cpp',
'src/sysctl.cpp',
'src/symlink.cpp',
'src/time.cpp',
'src/touch.cpp',
'src/uid.cpp',
'src/uname.cpp',
'src/which.cpp',
'src/winsock.cpp',
'src/windows.cpp'
)

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
def_copy_file = []

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

else

# -- copy_file_range()

if not is_windows
  if cpp.has_function('copy_file_range', args: '-D_GNU_SOURCE', prefix: '#include <unistd.h>')
    def_copy_file = '-DHAVE_COPY_FILE_RANGE'
  endif
endif

endif # use_cpp

# -- C++ map.contains
cpp_map_contains = cpp.compiles('''#include <map>
int main(){
std::map<int, char> m{{1, 'a'}, {2, 'b'}};
return m.contains(2);
}''', name: 'C++ map.contains')

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
sources: [fsrcs, comm_src],
include_directories: [inc],
install: true,
c_args: [],
cpp_args: [have_filesystem, chrono_def, dladdr_def, getloadavg_def, mkdtemp_def, def_copy_file, trace_def],
fortran_args: f03type_def,
dependencies: [libwin, fs_lib, dl],
)

if not meson.is_subproject()

executable('libc_version', 'app/libc.cpp')
executable('libcpp_version', 'app/libcxx.cpp')

if cpp_map_contains
  executable('fs_cli', sources: 'app/main.cpp', link_with: filesystem, include_directories: inc, install: true)
endif

if fortran

executable('filesystem_cli',
  sources: 'app/fortran/main.f90',
  link_with: filesystem,
  install: true)

endif

endif

if not meson.is_subproject()
  foreach d : ['core', 'environment', 'executable']
    subdir('test/' + d)
  endforeach

  if cpp_map_contains
    executable('ffilesystem_bench', 'test/bench/ffilesystem_bench.cpp', link_with: filesystem, include_directories: inc, install: true)
  endif
endif

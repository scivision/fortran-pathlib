project('ffilesystem', ['c'],
version: '5.7.1',
default_options: ['c_std=c99', 'cpp_std=c++20', 'buildtype=release', 'b_ndebug=if-release'],
meson_version : '>=1.4.0')

use_cpp = get_option('cpp')
if use_cpp
  add_languages('cpp', native: false, required: true)
endif

fortran = get_option('fortran')
if fortran
  add_languages('fortran', native: false, required: true)
endif

fs = import('fs')

cc = meson.get_compiler('c')

if ['gcc', 'clang', 'intel-llvm'].contains(cc.get_id())
  add_project_arguments('-Werror=implicit-function-declaration', language: 'c')
endif

fsrcs = fortran ? files('src/common/fortran/filesystem.F90') : []

csrcs = files('src/common/common.c',
'src/common/compiler.c',
'src/common/cygwin.c',
'src/common/exepath.c',
'src/common/env.c',
'src/common/home.c',
'src/common/mkdtemp.c',
'src/common/libpath.c',
'src/common/owner.c',
'src/common/os.c',
'src/common/partition.c',
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
csrcs += files('src/c/copy.c',
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

# -- C++ Feature checks
chrono_def = []
mkdtemp_def = []

if use_cpp

fs_check = '''
#include <filesystem>
static_assert(__cpp_lib_filesystem, "No C++ filesystem support");
int main () {
std::filesystem::path tgt(".");
auto h = tgt.has_filename();
return 0;
}'''

cpp = meson.get_compiler('cpp')
if not cpp.links(fs_check, name: 'C++ filesystem')
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
if cpp.links(chrono_src, name: 'C++20 chrono')
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
is_windows = host_machine.system() == 'windows'

libwin = []
win32_symlink_def = []

if is_windows

libwin = [cc.find_library('Userenv', required: true), cc.find_library('ws2_32', required: true)]

if use_cpp
  symlink_run = cpp.run(fs.read('cmake/fs_check/check_fs_symlink.cpp'), name: 'Symlink')
  if symlink_run.returncode() != 0
    win32_symlink_def = '-DWIN32_SYMLINK'
  endif
endif

endif

# -- main library

filesystem = library('filesystem',
sources: [fsrcs, csrcs, cppsrcs],
include_directories: [cwalk_inc, inc],
install: true,
c_args: [mkdtemp_def],
cpp_args: [chrono_def, mkdtemp_def, win32_symlink_def],
fortran_args: f03type_def,
dependencies: libwin,
link_with: cwalk_dep
)

if use_cpp
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

  if use_cpp
    executable('ffilesystem_bench', 'test/bench/ffilesystem_bench.cpp', link_with: filesystem, include_directories: inc, install: true)
  endif
endif

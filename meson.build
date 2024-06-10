project('ffilesystem', ['c', 'cpp'],
version: '5.6.6',
default_options: ['cpp_std=c++20', 'buildtype=release', 'b_ndebug=if-release'],
meson_version : '>=1.4.0')

fortran = get_option('fortran')
if fortran
  add_languages('fortran', native: false, required: true)
endif

fsrcs = fortran ? files('src/common/fortran/filesystem.F90') : []

csrcs = files('src/common/common.c',
'src/common/cygwin.c',
'src/common/partition.c',
'src/common/dynamic.c')

cppsrcs = files(
'src/common/filesystem.cpp',
'src/common/c_ifc.cpp',
'src/common/ifc.cpp',
'src/common/inquire.cpp',
'src/common/mkdir.cpp',
'src/common/pure.cpp',
'src/common/platform.cpp',
'src/common/resolve.cpp',
'src/common/symlink.cpp'
)

inc = include_directories('include')

# feature checks
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
else
  chrono_def = []
endif

fc = meson.get_compiler('fortran')

f03type_src = '''
program main
type :: path_t
character(:), allocatable :: s
end type
end program
'''

have_f03type = fc.compiles(f03type_src, name: 'Fortran 2003 derived type')

f03type_def = have_f03type ? [] : '-DNO_F03TYPE'

# Windows-specific
is_windows = host_machine.system() == 'windows'

libwin = []
if is_windows
  libwin = cpp.find_library('Userenv', required: true)
endif

# main library
filesystem = library('filesystem',
sources: [fsrcs, csrcs, cppsrcs],
include_directories: inc,
install: true,
cpp_args: chrono_def,
fortran_args: f03type_def,
dependencies: libwin,
)

executable('fs_cli', sources: 'app/main.cpp', link_with: filesystem,
include_directories: inc, install: true)

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

  executable('ffilesystem_bench', 'test/bench/ffilesystem_bench.cpp',
    link_with: filesystem, include_directories: inc, install: true)
endif

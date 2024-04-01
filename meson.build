project('ffilesystem', ['c', 'cpp'],
version: '5.6.0',
default_options: ['cpp_std=c++20'],
meson_version : '>=0.57.0')

fortran = get_option('fortran')
if fortran
  add_languages('fortran', native: false, required: true)
endif

fsrcs = fortran ? files('src/common/fortran/filesystem.f90') : []
csrcs = files('src/common/common.c')
cppsrcs = files(
'src/common/filesystem.cpp',
'src/common/dynamic.cpp',
'src/common/inquire.cpp',
'src/common/pure.cpp',
'src/common/platform.cpp',
'src/common/resolve.cpp',
'src/common/symlink.cpp',
'src/common/windows.cpp'
)

inc = include_directories('include')

fs_check = '''
#include <filesystem>
static_assert(__cpp_lib_filesystem, "No C++ filesystem support");
int main () {
std::filesystem::path tgt(".");
auto h = tgt.has_filename();
return 0;
}'''

cpp = meson.get_compiler('cpp')
if not cpp.links(fs_check, name: 'fs_check')
  error('No C++ filesystem support')
endif

filesystem = library('filessystem',
sources: [fsrcs, csrcs, cppsrcs],
include_directories: inc)

executable('fs_cli', sources: 'app/main.cpp', link_with: filesystem,
include_directories: inc, install: true)

if fortran
executable('filesystem_cli',
  sources: 'app/fortran/main.f90',
  link_with: filesystem,
  install: true)
endif

if not meson.is_subproject()
    test_core_cpp = executable('core_cpp', 'test/core/test_core.cpp', link_with: filesystem, include_directories: inc)
    test('core_cpp', test_core_cpp)

    if fortran
    test_core_fortran = executable('core_fortran', 'test/core/test_core.f90', link_with: filesystem)
    test('core_fortran', test_core_fortran)

    test_mkdtemp = executable('test_mkdtemp', 'test/mkdtemp/test_mkdtemp.f90', link_with: filesystem)
    test('test_mkdtemp', test_mkdtemp)
    endif
endif

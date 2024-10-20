#include "ffilesystem.h"

#include <string>
#include <iostream>

#include <optional>
#include <system_error> // for std::error_code

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <direct.h> // _getcwd, _chdir
#include <windows.h> // GetTempPathA
#else
#include <unistd.h> // getcwd, chdir
#endif


std::optional<std::string> fs_get_tempdir()
{
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  if(auto p = std::filesystem::temp_directory_path(ec); !ec) FFS_LIKELY
    return p.generic_string();

  std::cerr << "ERROR:ffilesystem:get_tempdir: " << ec.message() << "\n";
  return {};
#else

  std::string path(fs_get_max_path(), '\0');
#ifdef _WIN32
  auto L = GetTempPathA((DWORD) path.size(), path.data());
  if (L == 0 || L >= path.size()){
    fs_print_error(path, "get_tempdir");
    return {};
  }
  path.resize(L);
#else
  path = fs_getenv("TMPDIR");
#endif

  if(!path.empty())
    return fs_as_posix(path);

  if (fs_is_dir("/tmp"))
    return "/tmp";

  return {};
#endif
}


bool fs_set_cwd(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  std::filesystem::current_path(path, ec);
  if(!ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:chdir: " << ec.message() << "\n";
#else
  // unistd.h / direct.h
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/chdir-wchdir?view=msvc-170
  if(chdir(path.data()) == 0)
    return true;

  fs_print_error(path, "set_cwd");
#endif
  return false;
}


std::optional<std::string> fs_get_cwd()
{
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  if(auto s = std::filesystem::current_path(ec); !ec) FFS_LIKELY
    return s.generic_string();

  std::cerr << "ERROR:ffilesystem:get_cwd: " << ec.message() << "\n";
#else
// direct.h https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getcwd-wgetcwd
// unistd.h https://www.man7.org/linux/man-pages/man3/getcwd.3.html
  if(std::string buf(fs_get_max_path(), '\0');
      getcwd(buf.data(), buf.size()))
    return fs_as_posix(fs_trim(buf));

  fs_print_error("", "get_cwd");
#endif
  return {};
}

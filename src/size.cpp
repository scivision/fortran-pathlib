#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <string_view>
#include <system_error>
#include <set>
#include <iostream>

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>  // opendir, readdir, closedir
#endif


std::uintmax_t fs_file_size(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  if(auto s = std::filesystem::file_size(path, ec); !ec)  FFS_LIKELY
    return s;

  std::cerr << "ERROR:ffilesystem:file_size: " << ec.message() << "\n";
  return {};
#else
  if (struct stat s;
        !stat(path.data(), &s))  FFS_LIKELY
    return s.st_size;

  fs_print_error(path, "file_size");
  return {};
#endif
}


bool fs_is_empty(std::string_view path)
{
  // directory or file empty
  // returns false if path doesn't exist
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;

  return std::filesystem::is_empty(path, ec) && !ec;

#else

  const std::set<std::string_view> dots = {".", ".."};

  if (fs_is_dir(path))
  {
    if(FS_TRACE) std::cout << "TRACE: is_empty: checking directory " << path << "\n";
    // directory empty
#ifdef _MSC_VER
    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfilea
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA((std::string(path) + "/*").data(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
      fs_print_error(path, "is_empty:FindFirstFile");
      return false;
    }

    do
    {
        if(FS_TRACE) std::cout << "TRACE: is_empty: do " << ffd.cFileName << "\n";
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            if (
#if __cplusplus >= 202002L
            dots.contains(ffd.cFileName)
#else
            dots.find(ffd.cFileName) != dots.end()
#endif
            )
              continue;
        // directory that is not . or ..
        {
            FindClose(hFind);
            return false;
        }
        // any non-directory
        FindClose(hFind);
        return false;
    } while (FindNextFileA(hFind, &ffd));

    // empty directory
    FindClose(hFind);
    return true;
#else
// https://www.man7.org/linux/man-pages/man3/opendir.3.html
// https://www.man7.org/linux/man-pages/man3/readdir.3.html
// https://www.man7.org/linux/man-pages/man3/closedir.3.html
// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/readdir.3.html

    if (DIR *d = opendir(path.data()); d)
    {
      struct dirent *entry;
    while ((entry = readdir(d)))
    {
#ifdef _DIRENT_HAVE_D_TYPE
      if (entry->d_type == DT_DIR)
#else
      if (fs_is_dir(std::string(path) + "/" + entry->d_name))
#endif
      {
#if __cplusplus >= 202002L
        if (dots.contains(entry->d_name))
#else
        if (dots.find(entry->d_name) != dots.end())
#endif
          continue;
        // directory that is not . or ..
        closedir(d);
        return false;
      }
      // any non-directory
      closedir(d);
      return false;
    }
      // empty directory
      closedir(d);
      return true;
    }
    // not a directory
    fs_print_error(path, "is_empty:opendir");
    return false;

#endif
  }
  else {
    return fs_exists(path) && fs_file_size(path) == 0;
  }

#endif

}

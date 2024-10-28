#include "ffilesystem.h"

#include <string>
#include <iostream>
#include <system_error>         // for error_code
#include <vector>

#ifndef HAVE_CXX_FILESYSTEM
// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>  // mkdir

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

#endif


bool fs_mkdir(std::string_view path)
{
 // for clarity
  if(path.empty()){
    std::cerr << "ERROR:Ffs:mkdir empty path\n";
    return false;
  }

#ifdef HAVE_CXX_FILESYSTEM

  std::error_code ec;
  // old MacOS return false even if directory was created
  if (std::filesystem::create_directories(path, ec) || (!ec && fs_is_dir(path))) FFS_LIKELY
    return true;

  std::cerr << "ERROR:Ffs:mkdir(" << path << ") " << ec.message() << "\n";
  return false;

#else

  std::string buf;

  const std::vector<std::string> parts = fs_split(path);

  // if first part is root
  if(path[0] == '/' || (fs_is_windows() && path[0] == '\\'))
    buf = "/";

  // iterate over parts
  for(const auto& p : parts){
    buf += p + "/";
    if(FS_TRACE) std::cout << "TRACE:mkdir " << buf << "\n";
    // create directory
#ifdef _WIN32
    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createdirectorya
    if (CreateDirectoryA(buf.data(), nullptr) == 0){
      const auto err = GetLastError();
      if (!(err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED)){ FFS_UNLIKELY
        fs_print_error(buf, "mkdir:CreateDirectory");
        return false;
      }
    }
#else
    if (mkdir(buf.data(), S_IRWXU) && !(errno == EEXIST || errno == EACCES)){  FFS_UNLIKELY
      fs_print_error(buf, "mkdir");
      return false;
    }
#endif
  }

  if(fs_is_dir(path))  FFS_LIKELY
    return true;

  fs_print_error(path, "mkdir: not created");
  return false;

#endif
}

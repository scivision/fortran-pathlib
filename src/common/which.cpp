#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <string>
#include <iostream>

#include "ffilesystem.h"


std::string fs_which(std::string_view name)
{

  if (fs_is_exe(name))
    return fs_as_posix(name);

  // relative directory component, but path was not a file
  if(fs_file_name(name).length() != name.length())
    return {};

  std::string path = fs_getenv("PATH");
  if(path.empty()){
    fs_print_error(path, "which: PATH environment variable not set");
    return {};
  }

  if(FS_TRACE) std::cout << "TRACE:which: PATH: " << path << "\n";

  std::string n(name);
  std::string r;

  std::string_view::size_type start = 0;
  std::string_view::size_type end;

  do {
    end = path.find(fs_pathsep(), start);
    std::string p = path.substr(start, end - start);

    r = p + "/" + n;

    if (FS_TRACE) std::cout << "TRACE:which: is_file(" << r << ") " << fs_is_file(r) << " is_exe(" << r << ") " << fs_is_exe(r) << "\n";

    if (fs_is_exe(r))
      return fs_as_posix(r);

    start = end + 1;
  } while (end != std::string::npos);

  return {};
}

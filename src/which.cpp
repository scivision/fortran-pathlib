#include <string>
#include <string_view>

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
    fs_print_error("which: PATH environment variable not set", path);
    return {};
  }

  if(fs_trace) std::cout << "TRACE:which: PATH: " << path << "\n";

  std::string n(name);
  std::string r;

  std::string::size_type start = 0;
  std::string::size_type end;

  while (true) {
    end = path.find(fs_pathsep(), start);

    std::string p = (end == std::string::npos)
      ? path.substr(start)
      : path.substr(start, end - start);

    r = p + "/" + n;

    if (fs_trace) std::cout << "TRACE:which: is_file(" << r << ") " << fs_is_file(r) << " is_exe(" << r << ") " << fs_is_exe(r) << "\n";

    if (fs_is_exe(r))
      return fs_as_posix(r);

    if(end == std::string::npos)
      break;

    start = end + 1;
  }

  return {};
}

#include <string>
#include <string_view>

#include <iostream>

#include "ffilesystem.h"


std::string fs_which(std::string_view name, std::string_view path)
{
  // find an executable file "name" under each path in
  // environment variable PATH or "path" if specified

  if (fs_is_exe(name))
    return fs_as_posix(name);

  // relative directory component, but path was not a file
  if(fs_file_name(name).length() != name.length())
    return {};

  std::string paths = path.empty() ? fs_getenv("PATH") : std::string(path);

  if(paths.empty()){
    fs_print_error(paths, "which: search path is empty");
    return {};
  }

  paths = fs_as_posix(paths);

  if(fs_trace) std::cout << "TRACE:which: search path: " << paths << "\n";

  std::string n(name);
  std::string r;

  std::string::size_type start = 0;
  std::string::size_type end;

  while (true) {
    end = paths.find(fs_pathsep(), start);

    std::string dir = (end == std::string::npos)
      ? paths.substr(start)
      : paths.substr(start, end - start);

    r = dir + "/" + n;

    if (fs_trace) std::cout << "TRACE:which: is_file(" << r << ") " << fs_is_file(r) << " is_exe(" << r << ") " << fs_is_exe(r) << "\n";

    if (fs_is_exe(r))
      return fs_as_posix(r);

    if(end == std::string::npos)
      break;

    start = end + 1;
  }

  return {};
}

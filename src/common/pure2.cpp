// transition file for pure.c

#include <string>
#include <cctype>

#include "ffilesystem.h"


// pure.cpp
//------------------
bool fs_is_absolute(std::string_view path)
{
  if(path.empty())
    return false;

  if(fs_is_windows())
    return path.length() > 2 && std::isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || (fs_is_windows() && path[2] == '\\'));
  else
    return path[0] == '/';
}


std::string fs_root(std::string_view path)
{
  // our root = root_name / root_directory
  // empty path is not absolute

  if(!fs_is_absolute(path))
    return {};

  return fs_is_windows() ? std::string(1, path[0]) + ":/" : "/";
}

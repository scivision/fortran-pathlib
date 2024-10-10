// transition file for pure.c

#include <string>
#include <cctype>

#include "ffilesystem.h"


// tell if Ffilesystme core is C or C++
bool fs_cpp(){ return true; }

// C++ version compiler claims to support with given options
long fs_lang(){ return __cplusplus; }


bool fs_is_absolute(std::string_view path)
{
  if(path.empty())
    return false;

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).is_absolute();
#else
  if(fs_is_windows())
    return path.length() > 2 && std::isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || (fs_is_windows() && path[2] == '\\'));
  else
    return path[0] == '/';
#endif
}


std::string fs_file_name(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().generic_string();
#else
  std::string r = fs_as_posix(path);

  if (auto pos = r.find_last_of('/');
        pos != std::string::npos)
    return std::string(r.substr(pos + 1));

  return r;
#endif
}


std::string fs_root(std::string_view path)
{
  // our root = root_name / root_directory
  // empty path is not absolute
  if(!fs_is_absolute(path))
    return {};

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).root_path().generic_string();
#else
  return fs_is_windows() ? std::string(1, path[0]) + ":/" : "/";
#endif
}


std::string fs_stem(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().stem().generic_string();
#else
  std::string r = fs_file_name(path);
  if(r.empty())
    return {};

  // handle special case a/..
  if (r == "..")
    return r;

  // find last dot
  if (auto pos = r.find_last_of('.');
        pos != std::string::npos && pos != 0)
    return std::string(r.substr(0, pos));

  return r;
#endif
}

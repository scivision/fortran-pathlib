// transition file for pure.c

#include <string>
#include <cctype>
#include <vector>
#include <iostream>

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


std::vector<std::string> fs_split(std::string_view path)
{
  if(path.empty())
    return {};
  // break paths into components
  std::vector<std::string> parts;
  std::string p = fs_as_posix(path);
  // split path, including last component
  size_t start = 0;
  size_t end;
  do {
    end = p.find_first_of('/', start);
    parts.push_back(p.substr(start, end - start));
    start = end + 1;
  } while (end != std::string::npos);
  return parts;
}


std::string fs_normal(std::string_view path)
{
  // normalize path
#ifdef HAVE_CXX_FILESYSTEM
  return fs_drop_slash(std::filesystem::path(path).lexically_normal().generic_string());
#else
  const std::vector<std::string> parts = fs_split(path);
  if(parts.empty())
    return {};
  std::vector<std::string> new_parts;

  // drop repeated slashes, "." and ".."
  for (const auto& part : parts) {
    if (part.empty())
      continue;

    if (part == "."){
      if (new_parts.empty())
        new_parts.push_back(part);

      continue;
    }

    if (part == "..") {
      if (new_parts.empty() || new_parts.back() == "..")
        new_parts.push_back(part);
      else
        new_parts.pop_back();

      continue;
    }

    if(new_parts.size() == 1 && new_parts[0] == ".")
      new_parts.pop_back();

    new_parts.push_back(part);
  }

  // rebuild path, without trailing slash
  std::string r;
  // preserve leading slash
  if (path[0] == '/')
    r = "/";

  for (const auto& part : new_parts){
    r += part + "/";
  }

  if(r.length() > 1 && r.back() == '/')
    r.pop_back();

  return r;
#endif
}


std::string fs_drop_slash(std::string_view sv)
{
  // drop all trailing "/"
  std::string s(sv);
  while(s.length() > 1 && (s.back() == '/' || (fs_is_windows() && s.back() == '\\')))
    s.pop_back();
  return s;
}


std::string fs_relative_to(std::string_view base, std::string_view other)
{
  // find lexical relative path from base to other
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(other).lexically_relative(base).lexically_normal().generic_string();
#else

  if(base.empty() && other.empty())
    return ".";

  if(base.empty() || other.empty())
    return {};

  std::string b = fs_normal(base);
  std::string o = fs_normal(other);

  if(b == o)
    return ".";

  std::vector<std::string> parts = fs_split(b);
  std::vector<std::string> other_parts = fs_split(o);

  std::string::size_type Lb = parts.size();
  std::string::size_type Lo = other_parts.size();

  // find common prefix, returning empty if no common prefix
  size_t i = 0;
  for (; i < Lb && i < Lo; i++)
    if (parts[i] != other_parts[i])
      break;

  if (i == 0 && parts[0] != other_parts[0])
    return {};


  // build relative path

  std::string r;
  for (size_t j = i; j < parts.size(); j++)
    r += "../";

  for (size_t j = i; j < other_parts.size(); j++)
    r += other_parts[j] + "/";

  return fs_drop_slash(r);

#endif
}



std::string fs_proximate_to(std::string_view base, std::string_view other)
{
// proximate_to is LEXICAL operation
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(other).lexically_proximate(base).lexically_normal().generic_string();
#else
  std::string r = fs_relative_to(base, other);
  return (r.empty()) ? std::string(other) : r;
#endif
}

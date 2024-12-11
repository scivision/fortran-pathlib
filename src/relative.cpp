#include "ffilesystem.h"

#include <string>
#include <string_view>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <vector>
#include <iostream>
#endif


std::string fs_relative_to(std::string_view base, std::string_view other)
{
  // find lexical relative path from base to other
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(other).lexically_relative(base).generic_string();
#else

  if(base.empty() && other.empty())
    return ".";

  if(base.empty() || other.empty())
    return {};

  const std::vector<std::string> b = fs_split(fs_drop_slash(base));
  const std::vector<std::string> o = fs_split(other);

  if(b == o)
    return ".";

  const std::string::size_type Lb = b.size();
  const std::string::size_type Lo = o.size();

  // find common prefix, returning empty if no common prefix
  if(fs_trace) std::cout << "TRACE:relative_to: normal_vector lengths " << Lb << " " << Lo << "\n";
  std::string::size_type i = 0;
  for (; i < Lb && i < Lo; i++){
    if(fs_trace) std::cout << "TRACE:relative_to: " << b[i] << " " << o[i] << "\n";
    if (b[i] != o[i])
      break;
  }

  if (i == 0 && b[0] != o[0])
    return {};


  // build relative path

  std::string r;
  for (std::string::size_type j = i; j < Lb; j++)
    r.append("../");

  for (std::string::size_type j = i; j < Lo; j++)
    r.append(o[j]).push_back('/');

  r = fs_drop_slash(r);

  if(r == "/" || r.empty())
    r = ".";

  if (other.back() == '/' && r.back() != '/' && r != ".")
    r.push_back('/');

  return r;

#endif
}



std::string fs_proximate_to(std::string_view base, std::string_view other)
{
// proximate_to is LEXICAL operation
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(other).lexically_proximate(base).generic_string();
#else
  const std::string r = fs_relative_to(base, other);
  return (r.empty()) ? std::string(other) : r;
#endif
}


bool fs_is_subdir(std::string_view subdir, std::string_view dir)
{
  // is subdir a subdirectory of dir. Does not normalize, canonicalize,
  // or walk up the directory tree. A lexical operation only.

  if(subdir.empty() || dir.empty())
    return false;

  const std::string s = fs_drop_slash(subdir);
  const std::string d = fs_drop_slash(dir);

  if(s == d)
    return false;

  if(s.size() < d.size())
    return false;

  if(s.substr(0, d.size()) != d)
    return false;

  return true;

}

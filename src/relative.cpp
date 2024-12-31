#include "ffilesystem.h"

#include <string>
#include <string_view>

#if __has_include(<ranges>)
#include <ranges>
#endif

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <iostream>
#include <vector>
#include <algorithm> // std::find
#endif


std::string fs_relative_to(std::string_view base, std::string_view other)
{
  // find lexical relative path from base to other
  // ".." is ambiguous and should be avoided in input

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
  while (i < Lb && i < Lo) {
    if(fs_trace) std::cout << "TRACE:relative_to: " << b[i] << " " << o[i] << "\n";
    if (b[i] != o[i])
      break;
    i++;
  }

  if (i == 0)
    return {};

  // build relative path

  std::string r;
  for (std::string::size_type j = i; j < Lb; j++)
    r.append("../");

  for (std::string::size_type j = i; j < Lo; j++)
    r.append(o[j]).push_back('/');

  r = fs_drop_slash(r);

  if (r.empty() || r == "/")
    r = ".";

  if (other.back() == '/' && r.back() != '/' && r != ".")
    r.push_back('/');

  return r;

#endif
}



std::string fs_proximate_to(std::string_view base, std::string_view other)
{
// proximate_to is a LEXICAL operation
// A lexical operation means that the function operates purely on the string representation of the paths,
// without accessing the filesystem. It manipulates the paths as sequences of characters,
// ensuring that the result is a valid relative path string.
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(other).lexically_proximate(base).generic_string();
#else
  const std::string r = fs_relative_to(base, other);
  return (r.empty()) ? std::string(other) : r;
#endif
}

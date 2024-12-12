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

#ifdef __cpp_lib_ranges_contains  // C++23
  if (std::ranges::contains(b, ".."))
#elif __cpp_lib_ranges // C++20
  if (std::ranges::find(b, "..") != b.end())
#else // C++98
  if (std::find(b.begin(), b.end(), "..") != b.end())
#endif
    std::cerr << "relative_to(" << base << ", " << other << ") has ambiguous base with '..'  consider using fs_canonical() first\n";

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

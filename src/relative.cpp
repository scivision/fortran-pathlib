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
  return std::filesystem::path(other).lexically_relative(base).lexically_normal().generic_string();
#else

  if(base.empty() && other.empty())
    return ".";

  if(base.empty() || other.empty())
    return {};

  const std::vector<std::string> b = fs_normal_vector(base);
  const std::vector<std::string> o = fs_normal_vector(other);

  if(b == o)
    return ".";

  const std::string::size_type Lb = b.size();
  const std::string::size_type Lo = o.size();

  // find common prefix, returning empty if no common prefix
  if(FS_TRACE) std::cout << "TRACE:relative_to: normal_vector lengths " << Lb << " " << Lo << "\n";
  std::string::size_type i = 0;
  for (; i < Lb && i < Lo; i++){
    if(FS_TRACE) std::cout << "TRACE:relative_to: " << b[i] << " " << o[i] << "\n";
    if (b[i] != o[i])
      break;
  }

  if (i == 0 && b[0] != o[0])
    return {};


  // build relative path

  std::string r;
  for (std::string::size_type j = i; j < Lb; j++)
    r += "../";

  for (std::string::size_type j = i; j < Lo; j++)
    r += o[j] + "/";

  return fs_drop_slash(r);

#endif
}



std::string fs_proximate_to(std::string_view base, std::string_view other)
{
// proximate_to is LEXICAL operation
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(other).lexically_proximate(base).lexically_normal().generic_string();
#else
  const std::string r = fs_relative_to(base, other);
  return (r.empty()) ? std::string(other) : r;
#endif
}


bool fs_is_subdir(std::string_view subdir, std::string_view dir)
{
  // is subdir a subdirectory of dir

  const std::string r = fs_relative_to(dir, subdir);

  return !r.empty() && r != "." &&
#ifdef __cpp_lib_starts_ends_with // C++20
    !r.starts_with("..");
#else // C++98
    r.substr(0,2) != "..";
#endif

}

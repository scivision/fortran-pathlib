#include "ffilesystem.h"

#include <string>
#include <vector>
#include <iostream>


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

  const std::string b = fs_normal(base);
  const std::string o = fs_normal(other);

  if(b == o)
    return ".";

  const std::vector<std::string> parts = fs_split(b);
  const std::vector<std::string> other_parts = fs_split(o);

  const std::string::size_type Lb = parts.size();
  const std::string::size_type Lo = other_parts.size();

  // find common prefix, returning empty if no common prefix
  if(FS_TRACE) std::cout << "TRACE:relative_to: " << b << " " << Lb << " " << o << " " << Lo << "\n";
  size_t i = 0;
  for (; i < Lb && i < Lo; i++){
    if(FS_TRACE) std::cout << "TRACE:relative_to: " << parts[i] << " " << other_parts[i] << "\n";
    if (parts[i] != other_parts[i])
      break;
  }

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
  const std::string r = fs_relative_to(base, other);
  return (r.empty()) ? std::string(other) : r;
#endif
}


bool fs_is_subdir(std::string_view subdir, std::string_view dir)
{
  // is subdir a subdirectory of dir

  const std::string r = fs_relative_to(dir, subdir);

  return !r.empty() && r != "." &&
#ifdef __cpp_lib_starts_ends_with
    !r.starts_with("..");
#else
    r.substr(0,2) != "..";
#endif

}

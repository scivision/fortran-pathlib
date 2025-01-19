#include "ffilesystem.h"

#include <string>
#include <string_view>

#if __has_include(<ranges>)
#include <ranges>
#endif

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#include <system_error>
#else
#include <iostream>
#include <vector>
#include <algorithm> // std::find
#endif


std::string fs_relative_to(std::string_view base, std::string_view other)
{
  // find relative path from base to other
  // "base" and "other" are treated as weakly canonical paths

#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  std::string r = std::filesystem::relative(other, base, ec).generic_string();
  // this implements
  // std::filesystem::weakly_canonical(other, ec).lexically_relative(std::filesystem::weakly_canonical(base, ec)).generic_string();
  if(!ec)
    return fs_drop_slash(r);
#else
  fs_print_error("requires <filesystem> backend", "fs_relative_to");
#endif

  fs_print_error(base, other, "relative");
  return {};
}



std::string fs_proximate_to(std::string_view base, std::string_view other)
{
  // find proximate path from base to other
  // "base" and "other" are treated as weakly canonical paths

#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  std::string r = std::filesystem::proximate(other, base, ec).generic_string();
  if(!ec)
    return fs_drop_slash(r);
#else
  fs_print_error("requires <filesystem> backend", "fs_proximate_to");
#endif

  fs_print_error(base, other, "proximate");
  return {};
}

// This file contains pure or lexical functions
// that don't require additional includes

#include "ffilesystem.h"

#include <string>
#include <iostream>
#include <cctype>


std::string Ffs::lexically_normal(std::string_view path){
  return std::filesystem::path(path).lexically_normal().generic_string();
}


std::string Ffs::make_preferred(std::string_view path){
  return std::filesystem::path(path).make_preferred().generic_string();
}


std::string Ffs::parent(std::string_view path)
{
  // have to drop_slash on input to get expected parent path -- necessary for AppleClang
  std::string p = std::filesystem::path(fs_drop_slash(path)).parent_path().generic_string();

  // remove repeated path seperators from p string
  p.erase(std::unique(p.begin(), p.end(), [](char a, char b){ return a == '/' && b == '/'; }), p.end());

  if(FS_TRACE) std::cout << "TRACE:parent(" << path << ") => " << p << "\n";

// 30.10.7.1 [fs.path.generic] dot-dot in the root-directory refers to the root-directory itself.
// On Windows, a drive specifier such as "C:" or "z:" is treated as a root-name.
// On Cygwin, a path that begins with two successive directory separators is a root-name.
// Otherwise (for POSIX-like systems other than Cygwin), the implementation-defined root-name
// is an unspecified string which does not appear in any pathnames.

  // handle "/" and other no parent cases
  if (p.empty()){
    if (!path.empty() && path.front() == '/')
      return "/";
    else
      return ".";
  }

  if (fs_is_windows() && p.length() == 2 && std::isalpha(p[0]) && p[1] == ':')
    return p + "/";

  return p;
}


std::string Ffs::suffix(std::string_view path){
  return std::filesystem::path(path).filename().extension().generic_string();
}


std::string Ffs::with_suffix(std::string_view path, std::string_view new_suffix){
  return std::filesystem::path(path).replace_extension(new_suffix).generic_string();
}

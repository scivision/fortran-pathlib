// This file contains pure or lexical functions
// that don't require additional includes

#include "ffilesystem.h"

#include <string>
#include <iostream>


// tell if Ffilesystme core is C or C++
bool fs_cpp(){ return true; }

// C++ version compiler claims to support with given options
long fs_lang(){ return __cplusplus; }


std::string::size_type fs_str2char(std::string_view s, char* result, const std::string::size_type buffer_size)
{
  if(s.length() >= buffer_size) FFS_UNLIKELY
  {
    result = nullptr;
    std::cerr << "ERROR:Ffs:str2char(" << s << ") output buffer " << buffer_size << " too small for string: " << s << "\n";
    return 0;
  }

  s.copy(result, buffer_size);
  result[s.length()] = '\0';
  return s.length();
}


std::string Ffs::drop_slash(std::string_view sv)
{
  // drop single final "/" if present
  std::string s(sv);

  if (s.length() > 1 && s.back() == '/')
    s.pop_back();

  return s;
}


std::string Ffs::lexically_normal(std::string_view path){
  return std::filesystem::path(path).lexically_normal().generic_string();
}


std::string Ffs::make_preferred(std::string_view path){
  return std::filesystem::path(path).make_preferred().generic_string();
}


std::string Ffs::normal(std::string_view path){
  return Ffs::drop_slash(std::filesystem::path(path).lexically_normal().generic_string());
}


std::string Ffs::file_name(std::string_view path){
  return std::filesystem::path(path).filename().generic_string();
}


std::string Ffs::stem(std::string_view path){
  return std::filesystem::path(path).filename().stem().generic_string();
}


std::string Ffs::join(std::string_view path, std::string_view other){
  return Ffs::drop_slash((std::filesystem::path(path) / other).lexically_normal().generic_string());
}


std::string Ffs::parent(std::string_view path)
{
  // have to drop_slash on input to get expected parent path
  std::string p = std::filesystem::path(Ffs::drop_slash(path)).parent_path().generic_string();

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

  return p;
}


std::string Ffs::suffix(std::string_view path){
  return std::filesystem::path(path).filename().extension().generic_string();
}


std::string Ffs::with_suffix(std::string_view path, std::string_view new_suffix){
  return std::filesystem::path(path).replace_extension(new_suffix).generic_string();
}


std::string Ffs::root(std::string_view path){
  auto p = std::filesystem::path(path);

  if(p.is_absolute())
    return p.root_path().generic_string();

  return {};
}


bool Ffs::is_absolute(std::string_view path){
  return std::filesystem::path(path).is_absolute();
}


// relative_to is LEXICAL operation
std::string Ffs::relative_to(std::string_view base, std::string_view other){
  return std::filesystem::path(other).lexically_relative(base).lexically_normal().generic_string();
}


// proximate_to is LEXICAL operation
std::string Ffs::proximate_to(std::string_view base, std::string_view other){
  return std::filesystem::path(other).lexically_proximate(base).lexically_normal().generic_string();
}

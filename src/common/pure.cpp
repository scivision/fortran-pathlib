// This file contains pure or lexical functions
// that don't require additional includes

#include "ffilesystem.h"

#include <set>
#include <iostream>

#if __has_include(<ranges>)
#include <ranges>
#endif

#include <cstddef> // size_t
#include <cctype> // std::isalnum


// tell if Ffilesystme core is C or C++
bool fs_cpp(){ return true; }

// C++ version compiler claims to support with given options
long fs_lang(){ return __cplusplus; }


size_t fs_str2char(std::string_view s, char* result, size_t buffer_size)
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


std::string fs_drop_slash(std::string_view sv)
{
  // drop single final "/" if present
  std::string s(sv);

  if (s.length() > 1 && s.back() == '/')
    s.pop_back();

  return s;
}


void fs_as_posix(char* path)
{
  std::string p = Ffs::as_posix(std::string_view(path));
  fs_str2char(p, path, p.length()+1);
}

std::string Ffs::as_posix(std::string_view path){
  // force posix file separator on Windows
  return std::filesystem::path(path).generic_string();
}


std::string Ffs::lexically_normal(std::string_view path){
  return std::filesystem::path(path).lexically_normal().generic_string();
}


std::string Ffs::make_preferred(std::string_view path){
  return std::filesystem::path(path).make_preferred().generic_string();
}


std::string Ffs::normal(std::string_view path){
  return fs_drop_slash(std::filesystem::path(path).lexically_normal().generic_string());
}


std::string Ffs::file_name(std::string_view path){
  return std::filesystem::path(path).filename().generic_string();
}


std::string Ffs::stem(std::string_view path){
  return std::filesystem::path(path).filename().stem().generic_string();
}


std::string Ffs::join(std::string_view path, std::string_view other){
  return fs_drop_slash((std::filesystem::path(path) / other).lexically_normal().generic_string());
}


std::string Ffs::parent(std::string_view path)
{
  // have to drop_slash on input to get expected parent path
  std::string p = std::filesystem::path(fs_drop_slash(path)).parent_path().generic_string();

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
  return std::filesystem::path(path).root_path().generic_string();
}


bool Ffs::is_absolute(std::string_view path){
  return std::filesystem::path(path).is_absolute();
}


static bool fs_is_safe_char(const char c)
{
  std::set<char, std::less<>> safe = {'_', '-', '.', '~', '@', '#', '$', '%', '^', '&', '(', ')', '[', ']', '{', '}', '+', '=', ',', '!'};

  return std::isalnum(c) ||
#if __cplusplus >= 202002L
    safe.contains(c);
#else
    safe.find(c) != safe.end();
#endif

}


bool Ffs::is_safe_name(std::string_view filename)
{
  // check that only shell-safe characters are in filename using std::isalnum and a c++ set
  // hasn't been fully tested yet across operating systems and file systems--let us know if character(s) should be unsafe
  // does NOT check for reserved or device names
  // => filenames only, not paths
  // https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
  // we do not consider whitespaces, quotes, or ticks safe, as they can be confusing in shell scripts and command line usage

  // empty check for MSVC
  if(fs_is_windows() && !filename.empty() && filename.back() == '.') FFS_UNLIKELY
    return false;

#ifdef __cpp_lib_ranges
  return std::ranges::all_of(filename, fs_is_safe_char);
#else
  return std::all_of(filename.begin(), filename.end(), fs_is_safe_char);
#endif
}


// relative_to is LEXICAL operation
std::string Ffs::relative_to(std::string_view base, std::string_view other){
  return std::filesystem::path(other).lexically_relative(base).lexically_normal().generic_string();
}


// proximate_to is LEXICAL operation
std::string Ffs::proximate_to(std::string_view base, std::string_view other){
  return std::filesystem::path(other).lexically_proximate(base).lexically_normal().generic_string();
}


bool Ffs::is_subdir(std::string_view subdir, std::string_view dir)
{
  auto r = Ffs::relative_to(dir, subdir);

  return !r.empty() && r != "." &&
#ifdef __cpp_lib_starts_ends_with
    !r.starts_with("..");
#else
    (r[0] != '.' || (r.length() > 1 && r[1] != '.'));
#endif
}

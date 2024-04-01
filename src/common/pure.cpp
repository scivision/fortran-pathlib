// This file contains pure or lexical functions
// that don't require additional includes

#include "ffilesystem.h"

#include <set>
#include <iostream>

#if __has_include(<ranges>)
#include <ranges>
#endif

#include <cstring> // std::strlen
#include <cstddef> // size_t


bool fs_cpp()
{
// tell if fs core is C or C++
  return true;
}

long fs_lang()
{
  return __cplusplus;
}


size_t fs_str2char(std::string_view s, char* result, size_t buffer_size)
{
  if(s.length() >= buffer_size) FFS_UNLIKELY
  {
    result = nullptr;
    std::cerr << "ERROR:ffilesystem: output buffer " << buffer_size << " too small for string: " << s << "\n";
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
  fs_str2char(p, path, std::strlen(path)+1);
}

std::string Ffs::as_posix(std::string_view path)
{
  // force posix file separator on Windows
  return std::filesystem::path(path).generic_string();
}


std::string Ffs::lexically_normal(std::string_view path)
{
  return std::filesystem::path(path).lexically_normal().generic_string();
}


std::string Ffs::make_preferred(std::string_view path)
{
  return std::filesystem::path(path).make_preferred().generic_string();
}


size_t fs_normal(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::normal(std::string_view(path)), result, buffer_size);
}

std::string Ffs::normal(std::string_view path)
{
  return fs_drop_slash(std::filesystem::path(path).lexically_normal().generic_string());
}


size_t fs_file_name(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::file_name(std::string_view(path)), result, buffer_size);
}

std::string Ffs::file_name(std::string_view path)
{
  return std::filesystem::path(path).filename().generic_string();
}


size_t fs_stem(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::stem(std::string_view(path)), result, buffer_size);
}

std::string Ffs::stem(std::string_view path)
{
  return std::filesystem::path(path).filename().stem().generic_string();
}


size_t fs_join(const char* path, const char* other, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::join(std::string_view(path), std::string_view(other)), result, buffer_size);
}

std::string Ffs::join(std::string_view path, std::string_view other)
{
  if (other.empty()) FFS_UNLIKELY
    return std::string(path);

  return fs_drop_slash((std::filesystem::path(path) / other).lexically_normal().generic_string());
}


size_t fs_parent(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::parent(std::string_view(path)), result, buffer_size);
}

std::string Ffs::parent(std::string_view path)
{

  if (path.empty()) FFS_UNLIKELY
    return {};

  std::string p = std::filesystem::path(fs_drop_slash(path)).parent_path().generic_string();

  if(FS_TRACE) std::cout << "TRACE:parent(" << path << ") => " << p << "\n";

  // handle "/" and other no parent cases
  if (p.empty()){
    if (path.front() == '/')
      return "/";
    else
      return ".";
  }

  return p;
}


size_t fs_suffix(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::suffix(std::string_view(path)), result, buffer_size);
}

std::string Ffs::suffix(std::string_view path)
{
  return std::filesystem::path(path).filename().extension().generic_string();
}


size_t fs_with_suffix(const char* path, const char* new_suffix,
                      char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::with_suffix(std::string_view(path), std::string_view(new_suffix)), result, buffer_size);
}

std::string Ffs::with_suffix(std::string_view path, std::string_view new_suffix)
{
  return std::filesystem::path(path).replace_extension(new_suffix).generic_string();
}


size_t fs_root(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::root(std::string_view(path)), result, buffer_size);
}

std::string Ffs::root(std::string_view path)
{
  return std::filesystem::path(path).root_path().generic_string();
}


bool fs_is_absolute(const char* path)
{
  return Ffs::is_absolute(std::string_view(path));
}

bool Ffs::is_absolute(std::string_view path)
{
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


bool fs_is_safe_name(const char* filename)
{
  return Ffs::is_safe_name(std::string_view(filename));
}

bool Ffs::is_safe_name(std::string_view filename)
{
  // check that only shell-safe characters are in filename using std::isalnum and a c++ set
  // hasn't been fully tested yet across operating systems and file systems--let us know if character(s) should be unsafe
  // does NOT check for reserved or device names
  // => filenames only, not paths
  // https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
  // we do not consider whitespaces, quotes, or ticks safe, as they can be confusing in shell scripts and command line usage

  if(filename.empty()) FFS_UNLIKELY
    return false;

  if(fs_is_windows() && filename.back() == '.') FFS_UNLIKELY
    return false;

#ifdef __cpp_lib_ranges
  return std::ranges::all_of(filename, fs_is_safe_char);
#else
  return std::all_of(filename.begin(), filename.end(), fs_is_safe_char);
#endif
}

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <string>
#include <cstring>
#include <iostream>

#include "ffilesystem.h"


// C++ version compiler claims to support with given options
long fs_lang(){ return __cplusplus; }


std::string fs_backend(){
#ifdef HAVE_CXX_FILESYSTEM
  return "<filesystem>";
#else
  return "C";
#endif
}


char fs_pathsep(){
  return fs_is_windows() ? ';' : ':';
}


const char* fs_devnull(){
  return fs_is_windows() ? "nul" : "/dev/null";
}


void fs_as_posix(char* path)
{
// force posix file seperator on Windows
  if(!(fs_is_windows() || fs_is_cygwin()))
    return;

  for (char* p = path; *p != '\0'; p++) {
    if (*p == '\\')
      *p = '/';
  }
}


std::string::size_type fs_strncpy(const char* path, char* result, const std::string::size_type buffer_size)
{
// check size before copy
  size_t L = std::strlen(path);
  if(L >= buffer_size){
    std::cerr << "ERROR:Ffilesystem:strncpy: output buffer " << buffer_size << " too small for length " << L << "\n";
    return 0;
  }

  if(L)
    std::strncpy(result, path, buffer_size);

  return L;
}


bool fs_is_absolute(std::string_view path)
{
  if(path.empty())
    return false;

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).is_absolute();
#else
  if(fs_is_windows())
    return path.length() > 2 && std::isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || (fs_is_windows() && path[2] == '\\'));
  else
    return path[0] == '/';
#endif
}


std::string fs_file_name(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().generic_string();
#else

  const auto i = path.find_last_of(fs_is_windows() ? "/\\" : "/");

  return (i != std::string::npos)
    ? std::string(path.substr(i + 1))
    : std::string(path);
#endif
}


std::string fs_root(std::string_view path)
{
  // our root = root_name / root_directory
  // empty path is not absolute
  if(!fs_is_absolute(path))
    return {};

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).root_path().generic_string();
#else
  return fs_is_windows() ? std::string(1, path[0]) + ":/" : "/";
#endif
}


std::string fs_stem(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().stem().generic_string();
#else
  std::string r = fs_file_name(path);
  if(r.empty())
    return {};

  // handle special case a/..
  if (r == "..")
    return r;

  // find last dot
  if (auto pos = r.find_last_of('.');
        pos != std::string::npos && pos != 0)
    return std::string(r.substr(0, pos));

  return r;
#endif
}


std::string fs_suffix(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().extension().generic_string();
#else
  const std::string p = fs_file_name(path);
  if(p.empty())
    return {};

  // find last dot
  if (auto pos = p.find_last_of('.');
        pos != std::string::npos && pos != 0)
    return std::string(p.substr(pos));

  return {};
#endif
}


std::string fs_join(std::string_view path, std::string_view other){
#ifdef HAVE_CXX_FILESYSTEM
  return fs_drop_slash((std::filesystem::path(path) / other).lexically_normal().generic_string());
#else
  if(path.empty() && other.empty())
    return {};

  if(path.empty())
    return fs_drop_slash(other);

  if(other.empty())
    return fs_drop_slash(path);

  if (other[0] == '/' || (fs_is_windows() && fs_is_absolute(other)))
    return fs_drop_slash(other);

  return fs_normal(std::string(path) + "/" + std::string(other));
#endif
}


std::string fs_with_suffix(std::string_view path, std::string_view new_suffix)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).replace_extension(new_suffix).generic_string();
#else

  const std::string parent = fs_parent(path);
  const std::string stem = fs_stem(path);

  // handle directory case: stem is empty
  if(stem.empty())
    return fs_join(path, new_suffix);

  const std::string r = (parent == ".") ? stem : fs_join(parent, stem);

  return r + std::string(new_suffix);

#endif
}


std::string fs_lexically_normal(std::string_view path){
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).lexically_normal().generic_string();
#else
  std::cerr << "ERROR:fs_lexically_normal(" << path << "): C++ <filesystem> not available\n";
  return {};
#endif
}


std::string fs_make_preferred(std::string_view path){
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).make_preferred().generic_string();
#else
  std::cerr << "ERROR:fs_make_preferred(" << path << "): C++ <filesystem> not available\n";
  return {};
#endif
}

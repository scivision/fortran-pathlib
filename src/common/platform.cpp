#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif


#include "ffilesystem.h"

#include <cstdlib>

#include <cstring> // std::strerror
#include <string>
#include <iostream>
#include <system_error> // for error_code


std::string Ffs::get_env(std::string_view name)
{
  if(auto r = std::getenv(name.data()); r)
    return r;

  return {};
}


bool Ffs::set_env(std::string_view name, std::string_view value)
{

  if(
#ifdef _WIN32
  // SetEnvironmentVariable returned OK but set blank values
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/putenv-wputenv
  std::string v = std::string(name) + "=" + std::string(value); putenv(v.data()) == 0
#else
  // https://www.man7.org/linux/man-pages/man3/setenv.3.html
  setenv(name.data(), value.data(), 1) == 0
#endif
   ) return true;

  std::cerr << "Ffs:set_env: could not set environment variable " << name << "\n";
  return false;
}


std::string Ffs::get_tempdir()
{
  std::error_code ec;
  if(auto p = std::filesystem::temp_directory_path(ec); !ec) FFS_LIKELY
    return p.generic_string();

  std::cerr << "ERROR:ffilesystem:get_tempdir: " << ec.message() << "\n";
  return {};
}


std::string Ffs::get_cwd()
{
  std::error_code ec;
  if(auto s = std::filesystem::current_path(ec); !ec) FFS_LIKELY
    return s.generic_string();

  std::cerr << "ERROR:ffilesystem:get_cwd: " << ec.message() << "\n";
  return {};
}


bool Ffs::chdir(std::string_view path)
{
  std::error_code ec;
  if(std::filesystem::current_path(path, ec); !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:chdir: " << ec.message() << "\n";
  return false;
}

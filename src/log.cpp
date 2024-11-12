#include <system_error>

#include <iostream>

#include <string>
#include <string_view>

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cerrno>
#endif


void fs_print_error(std::string_view path, std::string_view fname)
{

  std::cerr << "ERROR:Ffilesystem:" << fname << "(" << path << ")  ";

#if defined(_WIN32) || defined(__CYGWIN__)
  const DWORD error = GetLastError();

  std::cerr << std::system_category().message(error) << "\n";

  if(error == ERROR_PRIVILEGE_NOT_HELD)
    std::cerr << "Enable Windows developer mode to use symbolic links: https://learn.microsoft.com/en-us/windows/apps/get-started/developer-mode-features-and-debugging \n";
#else
  auto econd = std::generic_category().default_error_condition(errno);
  std::cerr << econd.message() << "\n";
#endif
}


void fs_print_error(std::string_view path, std::string_view fname, const std::error_code& ec)
{
  std::cerr << "ERROR:Ffilesystem:" << fname << "(" << path << ")  ";
  if(ec)
    std::cerr << ec.message();

  std::cerr << "\n";
}

void fs_print_error(std::string_view path1, std::string_view path2, std::string_view fname, const std::error_code& ec)
{
  std::cerr << "ERROR:Ffilesystem:" << fname << "(" << path1 <<  ", " << path2 << ")  ";

  if(ec)
    std::cerr << ec.message();

  std::cerr << "\n";
}

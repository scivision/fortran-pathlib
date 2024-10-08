#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

#include <string>
#include <iostream>

#include "ffilesystem.h"

std::string fs_hostname()
{
  std::string name(fs_get_max_path(), '\0');

#if defined(_WIN32)
  WSADATA wsaData;

  if(WSAStartup(MAKEWORD(2, 0), &wsaData)){
    std::cerr << "ERROR:ffilesystem:fs_hostname: WSAStartup failed\n";
    return {};
  }

  int ret = gethostname(name.data(), (int) name.size());

  WSACleanup();
  if (ret == 0)
    return name;

  fs_print_error(name, "fs_hostname");
#else
  if (gethostname(name.data(), name.size()) == 0)
    return name;

  fs_print_error(name, "fs_hostname");
#endif
  return {};
}

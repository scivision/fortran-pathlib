#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "ffilesystem.h"

size_t fs_hostname(char* name, const size_t buffer_size)
{

#if defined(_WIN32)
  WSADATA wsaData;
  WORD ver = MAKEWORD(2, 0);
  if(WSAStartup(ver, &wsaData)){
    fprintf(stderr, "ERROR:ffilesystem:fs_hostname: WSAStartup failed\n");
    return 0;
  }

  int ret = gethostname(name, (int) buffer_size);

  WSACleanup();
  if (ret == 0)
    return strlen(name);

  fs_win32_print_error(name, "hostname");
#else
  if (gethostname(name, (int) buffer_size) == 0)
    return strlen(name);

  fprintf(stderr, "ERROR:ffilesystem:fs_hostname: %s\n", strerror(errno));
#endif
  return 0;
}

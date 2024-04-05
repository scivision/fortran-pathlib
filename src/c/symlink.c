#include "ffilesystem.h"

#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stddef.h> // size_t

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h> // readlink(), symlink()
#endif


bool fs_is_symlink(const char* path)
{
#ifdef _WIN32
  DWORD a = GetFileAttributes(path);
  return (a != INVALID_FILE_ATTRIBUTES) && (a & FILE_ATTRIBUTE_REPARSE_POINT);
#else
  struct stat buf;

  return !lstat(path, &buf) && S_ISLNK(buf.st_mode);
  // return (buf.st_mode & S_IFMT) == S_IFLNK; // equivalent
#endif
}


size_t fs_read_symlink(const char* path, char* result, size_t buffer_size)
{

#ifdef _WIN32
  (void) result;
  (void) buffer_size;
  fprintf(stderr, "ERROR:ffilesystem:read_symlink: not implemented for non-C++\n");
  return 0;
#else
  // https://www.man7.org/linux/man-pages/man2/readlink.2.html
  ssize_t Lr = readlink(path, result, buffer_size);
  if (Lr < 0){
    fprintf(stderr, "ERROR:ffilesystem:read_symlink: %s => %s\n", path, strerror(errno));
    return 0;
  }
  size_t L = (size_t) Lr;
  if (L == buffer_size) {
    fprintf(stderr, "ERROR:ffilesystem:read_symlink: truncation occurred %s\n", path);
    return 0;
  }
  // readlink() does not null-terminate the result
  result[L] = '\0';
  return L;
#endif
}


bool fs_create_symlink(const char* target, FFS_MUNUSED_C const char* link)
{

  // necessary to avoid logic problems on macOS
  if(!fs_exists(target)) {
    fprintf(stderr, "ERROR:filesystem:create_symlink: target path does not exist\n");
    return false;
  }

#ifdef _WIN32
  fprintf(stderr, "ERROR:ffilesystem:create_symlink: not implemented for non-C++\n");
  return false;
#else
  return symlink(target, link) == 0;
#endif
}
#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#if defined(__unix__)
// https://github.com/cpredef/predef/blob/master/OperatingSystems.md#bsd-environment
#include <sys/param.h>
#endif

#if defined(__linux__) && __has_include(<linux/magic.h>)
#include <sys/vfs.h>
#include <linux/magic.h>
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/magic.h
#elif defined(__APPLE__) || defined(BSD)
#include <sys/mount.h>
#elif defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdlib.h>  // malloc, free
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h> // isalpha

#include "ffilesystem.h"


#if defined(__linux__) && __has_include(<linux/magic.h>)
static inline const char* fs_type_linux(const char* path)
{
  struct statfs s;

  if(statfs(path, &s)) {
    fprintf(stderr, "ERROR:fs_get_type: statfs(%s) %s\n", path, strerror(errno));
    return "";
  }

  switch (s.f_type) {
    case BTRFS_SUPER_MAGIC: return "btrfs";
    case EXT4_SUPER_MAGIC: return "ext4";
#ifdef EXFAT_SUPER_MAGIC
    case EXFAT_SUPER_MAGIC: return "exfat";
#endif
#ifdef F2FS_SUPER_MAGIC
    case F2FS_SUPER_MAGIC: return "f2fs";
#endif
#ifdef FUSE_SUPER_MAGIC
    case FUSE_SUPER_MAGIC: return "fuse";
#endif
    case NFS_SUPER_MAGIC: return "nfs";
    case SQUASHFS_MAGIC: return "squashfs";
    case TMPFS_MAGIC: return "tmpfs";
#ifdef TRACEFS_MAGIC
    case TRACEFS_MAGIC: return "tracefs";
#endif
#ifdef UDF_SUPER_MAGIC
    case UDF_SUPER_MAGIC: return "udf";
#endif
    case V9FS_MAGIC: return "v9fs";
    // used for WSL
    // https://devblogs.microsoft.com/commandline/whats-new-for-wsl-in-windows-10-version-1903/
#ifdef XFS_SUPER_MAGIC
    case XFS_SUPER_MAGIC: return "xfs";
#endif
    default:
      fprintf(stderr, "ERROR:fs_get_type(%s) type ID: %ld\n", path, s.f_type);
      return "";
  }
}
#endif


size_t fs_filesystem_type(const char* path, char* name, const size_t buffer_size)
{
  // return name of filesystem type if known

#if defined(_WIN32) || defined(__CYGWIN__)
  const size_t MP = fs_get_max_path();

  char r[4];
  if(fs_is_cygwin()){
    // assume user input Windows path root directly.
    fs_strncpy(path, r, 4);
  } else {
    char* buf = (char*) malloc(MP);
    if(!buf) return 0;

    if (!fs_canonical(path, true, false, buf, MP) || !fs_root(buf, r, 4)) {
      free(buf);
      return 0;
    }
    free(buf);
  }

  // GetVolumeInformationA requires a trailing backslash
  if(isalpha(r[0]) && r[1] == ':') {
    if (r[2] == '\0')
      strcat(r, "\\");
    else if (r[2] == '/')
      r[2] = '\\';
  }

  if(FS_TRACE) printf("TRACE:get_type(%s) root: %s\n", path, r);

  size_t L;
  if(GetVolumeInformationA(r, NULL, 0, NULL, NULL, NULL, name, MAX_PATH+1)){
    L = strlen(name);
    if(L > 0 && L < buffer_size)
      return L;
  }

  fs_print_error(path, "filesystem_type");
  return 0;
#elif defined(__linux__)
#if __has_include(<linux/magic.h>)
  return fs_strncpy(fs_type_linux(path), name, buffer_size);
#else
  fs_print_error(path, "filesystem_type: linux/magic.h not found");
#endif
#elif defined(__APPLE__) || defined(BSD)
  struct statfs s;
  if(!statfs(path, &s))
    return fs_strncpy(s.f_fstypename, name, buffer_size);
#else
  fs_print_error(path, "filesystem_type: Unknown operating system");
#endif
  return 0;
}

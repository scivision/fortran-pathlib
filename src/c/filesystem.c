#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fileapi.h>
#include <io.h>
#include <direct.h> /* _mkdir, getcwd */
#include <sys/utime.h>
#else
#include <unistd.h>
#include <sys/time.h> // utimes
#endif


#if defined(__APPLE__) && defined(__MACH__)
#include <copyfile.h>
#endif

#include "ffilesystem.h"
#include <cwalk.h>


static inline bool _mkdir_segment(char* buf, size_t L) {
  // use mkdir() building up directory components using strtok()
// strtok_r, strtok_s not necessarily available, and non-C++ is fallback
  char* q = strtok(buf, "/");  // NOSONAR
  char* dir = (char*) malloc(L + 2);
  // + 2 to account for \0 and leading /
  if (!dir) {
    free(buf);
    return false;
  }

  dir[1] = '\0';
  dir[0] = (fs_is_windows()) ? '\0' : '/';

  while (q) {
    strcat(dir, q);
    if (FS_TRACE) printf("TRACE: mkdir %s\n", dir);

    if (
#ifdef _WIN32
      _mkdir(dir)
#else
      mkdir(dir, S_IRWXU)
#endif
        && errno != EEXIST) {
      fprintf(stderr, "ERROR:ffilesystem:create_directories: %s %s => %s\n", buf, dir, strerror(errno));
      free(buf);
      free(dir);
      return false;
    }
    strcat(dir, "/");
    q = strtok(NULL, "/"); // NOSONAR
  }

  free(dir);

  return true;
}


bool fs_mkdir(const char* path) {

  if(strlen(path) == 0) {
    fprintf(stderr, "ERROR:ffilesystem:create_directories: path must not be empty\n");
    return false;
  }

  if(fs_exists(path)){
    if(fs_is_dir(path))
      return true;

    fprintf(stderr, "ERROR:filesystem:create_directories: %s already exists but is not a directory\n", path);
    return false;
  }

  const size_t m = fs_get_max_path();

  // To disambiguate, use an absolute path -- must resolve multiple times because realpath only gives one level of non-existant path
  char* buf = (char*) malloc(m);
  if(!buf) return false;

  size_t L = fs_resolve(path, false, buf, m);
  if(L == 0){
    free(buf);
    return false;
  }

  if (FS_TRACE) printf("TRACE: mkdir %s resolved => %s\n", path, buf);
  if(!_mkdir_segment(buf, L))
    return false;

  /* check that path was adequately resolved and created */
  size_t L1 = fs_resolve(path, false, buf, m);

  const size_t max_depth = 1000;  // sanity check in case algorithm fails
  size_t i = 1;
  while (L1 != L) {
    if(!_mkdir_segment(buf, L1))
      return false;
    i++;
    if(i > max_depth) {
      fprintf(stderr, "ERROR:ffilesystem:create_directories: %s => too many iterations\n", path);
      free(buf);
      return false;
    }
    L = L1;
    L1 = fs_resolve(path, false, buf, m);
    if (FS_TRACE) printf("TRACE: mkdir %s iteration %zu resolved => %s   L1 %zu  L %zu\n", path, i, buf, L1, L);
  }

  bool ok = fs_is_dir(buf);
  free(buf);

  return ok;
}


bool fs_remove(const char* path)
{
  if (!fs_exists(path)){
    fprintf(stderr, "ERROR:ffilesystem:remove: %s does not exist\n", path);
    return false;
  }

#ifdef _WIN32
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-removedirectorya
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-deletefilea
  bool e = fs_is_dir(path) ? RemoveDirectoryA(path) : DeleteFileA(path);
  if (!e) {
    DWORD error = GetLastError();
    char *message;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		    NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *)&message, 0, NULL);
    fprintf(stderr, "ERROR:ffilesystem:remove: %s => %s\n", path, message);
  }
  return e;
#else
  return !remove(path);
#endif
}


bool fs_copy_file(const char* source, const char* dest, bool overwrite)
{

  if(overwrite && fs_is_file(dest) && !fs_remove(dest))
    fprintf(stderr, "ERROR:ffilesystem:copy_file: could not remove existing destination file %s\n", dest);

#if defined(_WIN32)
    if(!CopyFile(source, dest, true)){
      fprintf(stderr, "ERROR:ffilesystem:copy_file: could not copy file %s to %s\n", source, dest);
      return false;
    }
#elif defined(__APPLE__) && defined(__MACH__)
  /* copy-on-write file
  * based on kwSys:SystemTools:CloneFileContent
  * https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/copyfile.3.html
  * COPYFILE_CLONE is a 'best try' flag, which falls back to a copy if the clone fails.
  */
  if(copyfile(source, dest, NULL, COPYFILE_METADATA | COPYFILE_CLONE) < 0){
    fprintf(stderr, "ERROR:ffilesystem:copy_file: could not clone file %s to %s\n", source, dest);
    return false;
  }
#else
    // https://stackoverflow.com/a/29082484
    const int bufferSize = 4096;
    char buf[bufferSize];
    FILE *rid = fopen(source, "r");
    FILE *wid = fopen(dest, "w");

    if (rid == NULL || wid == NULL) {
      fprintf(stderr, "ERROR:ffilesystem:copy_file: could not open file %s or %s\n", source, dest);
      return false;
    }

    while (!feof(rid)) {
      size_t bytes = fread(buf, 1, sizeof(buf), rid);
      if (bytes)
        fwrite(buf, 1, bytes, wid);
    }

    fclose(rid);
    fclose(wid);
#endif

  return fs_is_file(dest);
}


bool fs_set_modtime(const char* path)
{
  if (
#ifdef _WIN32
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/utime-utime32-utime64-wutime-wutime32-wutime64
    _utime(path, NULL)
#else
    utimes(path, NULL)
#endif
    ){
      fprintf(stderr, "ERROR:Ffilesystem:set_modtime: %s => %s\n", path, strerror(errno));
      return false;
    }

  return true;
}


bool fs_touch(const char* path)
{
  if(strlen(path) == 0)
    return false;

  if(fs_exists(path))
    return fs_set_modtime(path);

  FILE* fid = fopen(path, "a+b");
  if(fid && fclose(fid) == 0)
    return true;

  fprintf(stderr, "ERROR:Ffilesystem:touch: %s => %s\n", path, strerror(errno));
  return false;
}


bool fs_set_permissions(const char* path, int readable, int writable, int executable)
{
  struct stat s;
  if(stat(path, &s))
    return false;
  if(s.st_mode & S_IFCHR)
    return false; // special POSIX file character device like /dev/null

#ifdef _MSC_VER
  int m = s.st_mode;
  const int r = _S_IREAD;
  const int w = _S_IWRITE
  const int x = _S_IEXEC;
#else
  mode_t m = s.st_mode;
  const mode_t r = S_IRUSR;
  const mode_t w = S_IWUSR;
  const mode_t x = S_IXUSR;
#endif

  if(readable > 0)
    m |= r;
  else if (readable < 0)
    m &= ~r;

  if(writable > 0)
    m |= w;
  else if (writable < 0)
    m &= ~w;

  if(executable > 0)
    m |= x;
  else if (executable < 0)
    m &= ~x;

  return chmod(path, m) == 0;
}


size_t fs_make_tempdir(char* result, size_t buffer_size)
{
#ifdef _WIN32
  (void) result; (void) buffer_size;
  fprintf(stderr, "ERROR:ffilesystem:fs_make_tempdir: not implemented for non-C++\n");
  return 0;
#else
  char tmpl[] = "tmp.XXXXXX";

  char* tmp = mkdtemp(tmpl);
  /* Linux: stdlib.h  macOS: unistd.h */
  if (!tmp){
    fprintf(stderr, "ERROR:filesystem:fs_make_tempdir:mkdtemp: could not create temporary directory %s\n", strerror(errno));
    return 0;
  }

  return fs_strncpy(tmp, result, buffer_size);
#endif
}

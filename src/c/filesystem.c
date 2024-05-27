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
#include <stdio.h>  // remove()

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <direct.h> // _mkdir
#include <sys/utime.h>
#define mkdir(dir, mode) _mkdir(dir)
#ifndef S_IRWXU
#define S_IRWXU (_S_IREAD | _S_IWRITE | _S_IEXEC)
#endif
#else
#include <unistd.h>
#include <sys/time.h> // utimes
#endif


#if defined(__APPLE__) && defined(__MACH__)
#include <copyfile.h>
#endif

#include "ffilesystem.h"
#include <cwalk.h>


bool fs_mkdir(const char* path)
{

  if(fs_is_dir(path))
    return true;

  struct cwk_segment segment;
  if(!cwk_path_get_first_segment(path, &segment))
    return false;

  const size_t m = fs_get_max_path();

  char *buf = (char*)malloc(m);
  if(!buf) return false;

  char r[4];
  size_t L = 0;

  if(fs_is_windows())
    r[0] = '\0';
  else
    fs_root(path, r, 4);
    // root may be empty for relative paths

  // printf("TRACE: mkdir(%s) %s %.*s\n", path, buf, (int)segment.size, segment.begin);
  do {
    if(L + segment.size + 1 >= m){
      fprintf(stderr, "ERROR:ffilesystem:mkdir: path too long %s\n", path);
      free(buf);
      return false;
    }
    L += snprintf(buf+L, m-L, "%s%.*s", r, (int)segment.size, segment.begin);
    if(r[0] != '/') {
      r[0] = '/';
      r[1] = '\0';
    }

    if(FS_TRACE) printf("TRACE: mkdir(%s) %.*s %zu %zu\n", buf, (int)segment.size, segment.begin, L, strlen(buf));

    if(mkdir(buf, S_IRWXU) && !(errno == EEXIST || errno == EACCES)) {
      fprintf(stderr, "ERROR:ffilesystem:mkdir: %s => %s\n", buf, strerror(errno));
      free(buf);
      return false;
    }

  } while(cwk_path_get_next_segment(&segment));

  free(buf);

  return fs_is_dir(path);
}


bool fs_remove(const char* path)
{

#ifdef _WIN32
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-removedirectorya
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-deletefilea
  const bool ok = fs_is_dir(path) ? RemoveDirectoryA(path) : DeleteFileA(path);
  if (!ok)
    fs_win32_print_error(path, "remove");

  return ok;
#else
  if(remove(path) == 0)
    return true;

  fprintf(stderr, "ERROR:Ffilesystem:remove: %s => %s\n", path, strerror(errno));
  return false;
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
  // on POSIX, only sets permission for user, not group or others

  struct stat s;
  if(stat(path, &s) || (s.st_mode & S_IFCHR))
    return false;
  // special POSIX file character device like /dev/null

#ifdef _MSC_VER
  int m = s.st_mode;
  const int r = _S_IREAD;
  const int w = _S_IWRITE;
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


size_t fs_make_tempdir(char* result, const size_t buffer_size)
{
  char tmpl[] = "tmp.XXXXXX";
  const char* tmp;

#ifdef _WIN32
  tmp = _mktemp(tmpl);
  if(!tmp){
    fprintf(stderr, "ERROR:filesystem:fs_make_tempdir:_mktemp: could not generate tempdir name %s\n", strerror(errno));
    return 0;
  }
  if(!fs_get_tempdir(result, buffer_size))
    return 0;

  const size_t L = fs_join(result, tmp, result, buffer_size);
  if(L == 0)
    return 0;

  fs_mkdir(result);
  return L;
#else
  tmp = mkdtemp(tmpl);
  /* Linux: stdlib.h  macOS: unistd.h */
  if (!tmp){
    fprintf(stderr, "ERROR:filesystem:fs_make_tempdir:mkdtemp: could not create temporary directory %s\n", strerror(errno));
    return 0;
  }

  return fs_strncpy(tmp, result, buffer_size);
#endif

}

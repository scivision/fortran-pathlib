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
#include <pwd.h>  /* getpwuid */
#include <unistd.h>
#include <sys/statvfs.h>
#include <sys/time.h>
#endif


#if defined(__APPLE__) && defined(__MACH__)
#include <copyfile.h>
#endif

#ifdef HAVE_UTSNAME_H
#include <sys/utsname.h>
#endif

#include "ffilesystem.h"
#include <cwalk.h>


#ifndef _WIN32
static inline bool str_ends_with(const char *s, const char *suffix) {
  /* https://stackoverflow.com/a/41652727 */
    size_t slen = strlen(s);
    size_t suffix_len = strlen(suffix);

    return suffix_len <= slen && !strcmp(s + slen - suffix_len, suffix);
}
#endif


int fs_is_wsl() {
#ifdef HAVE_UTSNAME_H
  struct utsname buf;
  if (uname(&buf) != 0)
    return false;

  if (strcmp(buf.sysname, "Linux") != 0)
    return 0;
  if (str_ends_with(buf.release, "microsoft-standard-WSL2"))
    return 2;
  if (str_ends_with(buf.release, "-Microsoft"))
    return 1;
#endif

  return 0;
}


bool fs_set_cwd(const char* path){

  if(strlen(path) == 0)
    return false;

  // <unistd.h>
  if(chdir(path) == 0)
    return true;

  fprintf(stderr, "ERROR:ffilesystem:set_cwd: %s    %s\n", path, strerror(errno));
  return false;

}


uintmax_t fs_file_size(const char* path)
{
  struct stat s;

  if (fs_is_file(path) && !stat(path, &s))
    return s.st_size;
  return 0;
}


uintmax_t fs_space_available(const char* path)
{
#ifdef _WIN32
  (void) path;
  fprintf(stderr, "ERROR:ffilesystem:space_available: not implemented for non-C++\n");
  return 0;
#else
  // sanity check
  if(!fs_exists(path))
    return 0;

  const size_t m = fs_get_max_path();

  char* r = (char*) malloc(m);
  if(!r)
    return 0;

  // for robustness and clarity, use root of path
  if (!fs_root(path, r, m)){
    fprintf(stderr, "ERROR:ffilesystem:space_available: %s => could not get root\n", path);
    free(r);
    return 0;
  }

  struct statvfs stat;

  if (statvfs(r, &stat)) {
    fprintf(stderr, "ERROR:ffilesystem:space_available: %s => %s\n", r, strerror(errno));
    free(r);
    return 0;
  }
  free(r);

  return stat.f_bsize * stat.f_bavail;

#endif
}


size_t fs_expanduser(const char* path, char* result, size_t buffer_size)
{
  // The path is also normalized by defintion
  if(path[0] != '~')
    return fs_normal(path, result, buffer_size);
  if(strlen(path) > 1 && path[1] != '/')
    return fs_normal(path, result, buffer_size);

  char* buf = (char*) malloc(buffer_size);
  if(!buf) return 0;
  if (!fs_get_homedir(buf, buffer_size)) {
    free(buf);
    return 0;
  }

  // ~ alone
  size_t L = strlen(path);
  if (L < 3){
    L = fs_normal(buf, result, buffer_size);
    if(FS_TRACE) printf("TRACE:expanduser: orphan ~: homedir %s %s\n", buf, result);
    free(buf);
    return L;
  }

  L = fs_join(buf, path+2, result, buffer_size);
  free(buf);

  return L;
}


bool fs_is_char_device(const char* path)
{
// special character device like /dev/null
// Windows: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fstat-fstat32-fstat64-fstati64-fstat32i64-fstat64i32
  struct stat s;
  return !stat(path, &s) && (s.st_mode & S_IFCHR);
  // S_ISCHR not available with MSVC
}


bool fs_is_dir(const char* path)
{
// NOTE: root() e.g. "C:" needs a trailing slash
  struct stat s;
  return !stat(path, &s) && (s.st_mode & S_IFDIR);
  // S_ISDIR not available with MSVC
}


bool fs_is_exe(const char* path)
{
  if (!fs_is_file(path))
    return false;

#ifdef _WIN32
  /* https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  * in Windows, all readable files are executable.
  * Do not use _S_IEXEC, it is not reliable.
  */
  return fs_is_readable(path);
#else
  return !access(path, X_OK);
#endif
}

bool fs_is_readable(const char* path)
{
/* directory or file readable */

#ifdef _WIN32
  /* https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  */
  return !_access_s(path, 4);
#else
  return !access(path, R_OK);
#endif
}

bool fs_is_writable(const char* path)
{
/* directory or file writable */

#ifdef _WIN32
  /* https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s
  */
  return !_access_s(path, 2);
#else
  return !access(path, W_OK);
#endif
}


bool fs_is_file(const char* path)
{
  struct stat s;

  return !stat(path, &s) && (s.st_mode & S_IFREG);
  // S_ISREG not available with MSVC
}


bool fs_is_reserved(const char* path)
{
  (void)path;
  if(fs_is_windows())
    fprintf(stderr, "ERROR:ffilesystem:is_reserved: not implemented without C++\n");
  return false;
}

bool fs_exists(const char* path)
{
  /* fs_exists() is true even if path is non-readable
  * this is like Python pathlib.Path.exists()
  * but unlike kwSys:SystemTools:FileExists which uses R_OK instead of F_OK like this project.
  */
  // false empty just for clarity
  return strlen(path) &&
#ifdef _MSC_VER
  /* kwSys:SystemTools:FileExists is much more elaborate with Reparse point checks etc.
  * For this project, Windows non-C++ is not officially supported so we do it simply.
  * This way seems to work fine on Windows anyway.
  */
   !_access_s(path, 0);
#else
  // <unistd.h>
   !access(path, F_OK);
#endif
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


size_t fs_get_permissions(const char* path, char* result, size_t buffer_size)
{
  if (buffer_size < 10) {
    fprintf(stderr, "ERROR:ffilesystem:fs_get_permissions: buffer_size must be at least 10\n");
    return 0;
  }

  struct stat s;

  if (stat(path, &s) != 0){
    fprintf(stderr, "ERROR:ffilesystem:fs_get_permissions: %s => %s\n", path, strerror(errno));
    return 0;
  }

#ifdef _MSC_VER
  (void) result;
  fprintf(stderr, "ERROR:ffilesystem:fs_get_permissions: not implemented for non-C++\n");
  return 0;
#else
  result[9] = '\0';
  result[0] = (s.st_mode & S_IRUSR) ? 'r' : '-';
  result[1] = (s.st_mode & S_IWUSR) ? 'w' : '-';
  result[2] = (s.st_mode & S_IXUSR) ? 'x' : '-';
  result[3] = (s.st_mode & S_IRGRP) ? 'r' : '-';
  result[4] = (s.st_mode & S_IWGRP) ? 'w' : '-';
  result[5] = (s.st_mode & S_IXGRP) ? 'x' : '-';
  result[6] = (s.st_mode & S_IROTH) ? 'r' : '-';
  result[7] = (s.st_mode & S_IWOTH) ? 'w' : '-';
  result[8] = (s.st_mode & S_IXOTH) ? 'x' : '-';
  return 9;
#endif
}


bool fs_touch(const char* path)
{
  if(strlen(path) == 0)
    return false;

  if(fs_exists(path)){
    if (
#ifdef _WIN32
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/utime-utime32-utime64-wutime-wutime32-wutime64
    _utime(path, NULL)
#else
    utimes(path, NULL)
#endif
    ){
      fprintf(stderr, "ERROR:Ffilesystem:touch: %s => %s\n", path, strerror(errno));
      return false;
    }
    return true;
  }

  FILE* fid = fopen(path, "a+b");
  if(!fid || fclose(fid)){
    fprintf(stderr, "ERROR:Ffilesystem:touch: %s => %s\n", path, strerror(errno));
    return false;
  }

  return true;
}


time_t fs_get_modtime(const char* path)
{
  struct stat s;

  if (fs_exists(path) && !stat(path, &s))
    return s.st_mtime;
  return 0;
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
  size_t L = strlen(tmp);
  if (L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:fs_make_tempdir: buffer_size %zu too small\n", buffer_size);
    return 0;
  }

  strncpy(result, tmp, buffer_size);
  return L;
#endif
}


/* environment variable functions */


size_t fs_getenv(const char* name, char* path, size_t buffer_size)
{
  // <stdlib.h>
  char* buf = getenv(name);
  if(!buf) // not error because sometimes we just check if envvar is defined
    return 0;

  size_t L = strlen(buf);
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:fs_getenv: buffer_size %zu is too small for %s\n", buffer_size, name);
    return 0;
  }

  strncpy(path, buf, buffer_size);
  return L;
}


bool fs_setenv(const char* name, const char* value)
{
  if(strlen(name) == 0)
    return false;

#ifdef _WIN32
  (void) value;
  fprintf(stderr, "ERROR:ffilesystem:fs_setenv: not implemented for non-C++\n");
#else
  if(!setenv(name, value, 1))
    return true;

  fprintf(stderr, "ERROR:ffilesystem:fs_setenv: %s => %s\n", name, strerror(errno));
#endif

  return false;
}


size_t fs_get_cwd(char* path, size_t buffer_size)
{
// direct.h https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getcwd-wgetcwd
// unistd.h https://www.man7.org/linux/man-pages/man3/getcwd.3.html
  if(!getcwd(path, buffer_size)){
    fprintf(stderr, "ERROR:ffilesystem:fs_get_cwd: %s\n", strerror(errno));
    return 0;
  }

  fs_as_posix(path);

  return strlen(path);
}

size_t fs_get_homedir(char* path, size_t buffer_size)
{
  size_t L = fs_getenv(fs_is_windows() ? "USERPROFILE" : "HOME", path, buffer_size);
  if (L){
    fs_as_posix(path);
    return L;
  }

#ifdef _WIN32
  return 0;
#else
  const char *h = getpwuid(geteuid())->pw_dir;
  if (!h)
    return 0;

  L = strlen(h);
  if (L >= buffer_size){
    fprintf(stderr, "ERROR:ffilesystem:fs_get_homedir: buffer_size %zu too small\n", buffer_size);
    return 0;
  }

  strncpy(path, h, buffer_size);
  return L;
#endif
}


size_t fs_get_tempdir(char* path, size_t buffer_size)
{
  size_t L = fs_getenv(fs_is_windows() ? "TEMP" : "TMPDIR", path, buffer_size);
  if(L)
    return L;

  if (buffer_size > 4 && fs_is_dir("/tmp")){
    strncpy(path, "/tmp", 5);
    return 4;
  }

  return 0;
}



bool fs_copy_file(const char* source, const char* dest, bool overwrite) {
  if(!fs_is_file(source)) {
    fprintf(stderr,"ERROR:ffilesystem:copy_file: source file does not exist %s\n", source);
    return false;
  }
  if(strlen(dest) == 0) {
    fprintf(stderr, "ERROR:ffilesystem:copy_file: destination path must not be empty\n");
    return false;
  }

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


static bool _mkdir_segment(char* buf, size_t L) {
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


/* stubs for non-implemented functions */

size_t fs_exe_path(char* path, size_t buffer_size)
{
  (void) path; (void) buffer_size;
  fprintf(stderr, "ERROR:ffilesystem:fs_exe_path: not implemented for non-C++\n");
  return 0;
}

size_t fs_lib_path(char* path, size_t buffer_size)
{
  (void) path; (void) buffer_size;
  fprintf(stderr, "ERROR:ffilesystem:fs_lib_path: not implemented for non-C++\n");
  return 0;
}

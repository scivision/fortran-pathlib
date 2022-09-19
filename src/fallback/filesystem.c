#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _MSC_VER
#include <stdlib.h>
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif


#include "filesystem.h"

bool is_macos(){
#if __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_MAC
  return true;
#endif
#endif
return false;
}

bool is_linux() {
#ifdef __linux__
  return true;
#endif
return false;
}

bool is_unix() {
#ifdef __unix__
  return true;
#endif
return false;
}

bool is_windows() {
#ifdef _WIN32
  return true;
#endif
return false;
}


bool sys_posix() {
  char sep[2];

  filesep(sep);
  return sep[0] == '/';
}

void filesep(char* sep) {
#ifdef _WIN32
  strcpy(sep, "\\");
#else
  strcpy(sep, "/");
#endif
}

size_t normal(const char* path, char* normalized) {
  if(path == NULL || strlen(path) == 0)
    return 0;

  size_t L=strlen(path);
  char* buf = (char*) malloc(L+1);  // +1 for null terminator
  strcpy(buf, path);

// force posix file seperator
  char s='\\';
  char *p = strchr(buf, s);
  while (p) {
      *p = '/';
      p = strchr(p+1, s);
  }

// dedupe file seperators
  int j=0;
  for (size_t i = 0; i < L; i++) {
    if (i<L-1 && buf[i] == '/' && buf[i + 1] == '/')
      continue;
    normalized[j] = buf[i];
    j++;
  }
  normalized[j] = '\0';

  free(buf);
  return strlen(normalized);
}

size_t file_name(const char* path, char* fname){

  if(path == NULL || strlen(path) == 0)
    return 0;

  char* buf = (char*) malloc(strlen(path) + 1);  // +1 for null terminator
  normal(path, buf);

  char* pos = strrchr(buf, '/');
  if (pos){
    strcpy(fname, pos+1);
  }
  else {
    strcpy(fname, buf);
  }

  free(buf);
  return strlen(fname);
}


uintmax_t file_size(const char* path) {
  struct stat s;

  if (!is_file(path)) return 0;

  if (stat(path, &s) == 0) return s.st_size;

  return 0;
}


bool is_dir(const char* path){
  struct stat s;

  int i = stat(path, &s);

  // NOTE: root() e.g. "C:" needs a trailing slash
  return i == 0 && (s.st_mode & S_IFDIR);
}


bool is_file(const char* path){
  struct stat s;

  int i = stat(path, &s);

  // NOTE: root() e.g. "C:" needs a trailing slash
  return i == 0 && (s.st_mode & S_IFREG);
}


bool is_exe(const char* path){
  struct stat s;

  if(stat(path, &s) != 0) return false;

#ifdef _MSC_VER
  return s.st_mode & _S_IEXEC;
#else
  return s.st_mode & S_IXUSR;
#endif
}


bool exists(const char* path) {
#ifdef _MSC_VER
  return _access_s(path, 0 ) == 0;
#else
  return access(path, F_OK) == 0;
#endif

}


size_t parent(const char* path, char* fparent){

  if(path == NULL || strlen(path) == 0) {
    strcpy(fparent, ".");
    return strlen(fparent);
  }

  char* buf = (char*) malloc(strlen(path) + 1);  // +1 for null terminator
  normal(path, buf);

  char* pos = strrchr(buf, '/');
  if (pos){
    strncpy(fparent, buf, pos-buf);
    fparent[pos-buf] = '\0';
  }
  else {
    strcpy(fparent, ".");
  }

  free(buf);
  return strlen(fparent);
}


size_t root(const char* path, char* r) {

if (is_absolute(path)){

#ifdef _WIN32
  strncpy(r, &path[0], 2);
  r[2] = '\0';
#else
  strncpy(r, &path[0], 1);
  r[1] = '\0';
#endif

}
else {
  r = "";
}

return strlen(r);
}


size_t stem(const char* path, char* fstem){

  if(path == NULL || strlen(path) == 0)
    return 0;

  char* buf = (char*) malloc(strlen(path) + 1);  // +1 for null terminator
  file_name(path, buf);

  char* pos = strrchr(buf, '.');
  if (pos && pos != buf){
    strncpy(fstem, buf, pos-buf);
    fstem[pos-buf] = '\0';
  }
  else {
    strcpy(fstem, buf);
  }

  free(buf);
  return strlen(fstem);
}


bool is_absolute(const char* path){
  if(path == NULL) return false;

  size_t L = strlen(path);
  if(L < 1) return false;

#ifdef _WIN32
  if(L < 2) return false;
  if(path[1] != ':') return false;
  if(!isalpha(path[0])) return false;
  return true;
#endif

  return path[0] == '/';
}


bool is_symlink(const char* path){
  if(path==NULL) return false;
  if(!exists(path)) return false;

#ifdef _WIN32
  return GetFileAttributes(path) & FILE_ATTRIBUTE_REPARSE_POINT;
#else
  struct stat buf;

  if(lstat(path, &buf) != 0) return false;

  // return (buf.st_mode & S_IFMT) == S_IFLNK; // equivalent to below line
  return S_ISLNK(buf.st_mode);
#endif
}

int create_symlink(const char* target, const char* link) {

#ifdef _WIN32
  if(is_dir(target)) {
    return !(CreateSymbolicLink(link, target,
      SYMBOLIC_LINK_FLAG_DIRECTORY | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE) != 0);
  }
  else {
    return !(CreateSymbolicLink(link, target,
      SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE) != 0);
  }
#else
  return symlink(target, link);
#endif

}


size_t get_cwd(char* path) {

#ifdef _MSC_VER
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getcwd-wgetcwd?view=msvc-170
  if (_getcwd(path, MAXP) == NULL) return 0;
#else
  if (getcwd(path, MAXP) == NULL) return 0;
#endif

  return normal(path, path);
}

bool fs_remove(const char* path) {
  if (!exists(path)) return true;

#ifdef _WIN32
  if (is_dir(path)){
    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-removedirectorya
    return RemoveDirectory(path) != 0;
  }
  else {
    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-deletefilea
    return DeleteFile(path) != 0;
  }
#else
  return remove(path) == 0;
#endif
}

bool chmod_exe(const char* path){
  struct stat s;
  if(stat(path, &s) != 0) return false;

#ifdef _MSC_VER
  return _chmod(path, s.st_mode | _S_IEXEC) == 0;
#else
  return chmod(path, s.st_mode | S_IXUSR) == 0;
#endif
}

bool chmod_no_exe(const char* path){
  struct stat s;
  if(stat(path, &s) != 0) return false;

#ifdef _MSC_VER
  return _chmod(path, s.st_mode | !_S_IEXEC) == 0;
#else
  return chmod(path, s.st_mode | !S_IXUSR) == 0;
#endif
}


size_t fs_realpath(const char* path, char* r) {
  if (path == NULL || strlen(path) == 0)
    return 0;

#ifdef _WIN32
  _fullpath(r, path, MAXP);
#else
  realpath(path, r);
#endif

  return strlen(r);
}

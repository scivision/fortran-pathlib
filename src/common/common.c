#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <stdbool.h>
#include <stdio.h>  // snprintf
#include <string.h>

// maximum path length
#if defined (__APPLE__)
#include <sys/syslimits.h>
#elif !defined (_MSC_VER)
#include <limits.h>
#endif
// end maximum path length

#if defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
// https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#include <sys/param.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>  // GetTokenInformation
#else
#include <unistd.h>  // geteuid
#include <sys/types.h>  // geteuid
#endif

size_t fs_get_max_path(){

  size_t m = 256;
#if defined(PATH_MAX)
  m = PATH_MAX;
#elif defined (_MAX_PATH)
  m = _MAX_PATH;
#elif defined (_POSIX_PATH_MAX)
  m = _POSIX_PATH_MAX;
#endif
  return (m < 4096) ? m : 4096; // arbitrary absolute maximum

}


bool fs_is_macos(){
#if defined(__APPLE__) && defined(__MACH__)
  return true;
#else
  return false;
#endif
}

bool fs_is_linux() {
#ifdef __linux__
  return true;
#else
  return false;
#endif
}

bool fs_is_unix() {
#ifdef __unix__
  return true;
#else
  return fs_is_macos();
#endif
}

bool fs_is_bsd() {
#ifdef BSD
  return true;
#else
  return false;
#endif
}

bool fs_is_windows() {
#ifdef _WIN32
  return true;
#else
  return false;
#endif
}

bool fs_is_cygwin(){
#ifdef __CYGWIN__
  return true;
#else
  return false;
#endif
}

bool fs_is_mingw(){
#ifdef __MINGW32__
  return true;
#else
  return false;
#endif
}

char fs_pathsep(){
  return fs_is_windows() ? ';' : ':';
}

const char* fs_devnull(){
  return fs_is_windows() ? "nul" : "/dev/null";
}


size_t fs_strncpy(const char* path, char* result, size_t buffer_size)
{
// check size before copy
  size_t L = strlen(path);
  if(L >= buffer_size){
    fprintf(stderr, "ERROR:Ffilesystem:strncpy: output buffer %zu too small\n", buffer_size);
    return 0;
  }

  if(L)
    strncpy(result, path, buffer_size);

  return L;
}


bool fs_is_admin(){
  // running as admin / root / superuser
#ifdef _WIN32
	HANDLE hToken = NULL;
	TOKEN_ELEVATION elevation;
	DWORD dwSize;

  bool ok = (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) &&
     GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize));

  CloseHandle(hToken);
  if(ok)
    return elevation.TokenIsElevated;

  return false;
#else
  return geteuid() == 0;
#endif
}


size_t fs_compiler(char* name, size_t buffer_size)
{
int L=0;

#if defined(__INTEL_LLVM_COMPILER)
  L = snprintf(name, buffer_size, "Intel LLVM %d %s", __INTEL_LLVM_COMPILER,  __VERSION__);
#elif defined(__NVCOMPILER_LLVM__)
  L = snprintf(name, buffer_size, "NVIDIA nvc %d.%d.%d", __NVCOMPILER_MAJOR__, __NVCOMPILER_MINOR__, __NVCOMPILER_PATCHLEVEL__);
#elif defined(__clang__)
  #ifdef __VERSION__
    L = snprintf(name, buffer_size, "Clang %s", __VERSION__);
  #else
    L = snprintf(name, buffer_size, "Clang %d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
  #endif
#elif defined(__GNUC__)
  L = snprintf(name, buffer_size, "GNU GCC %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
  L = snprintf(name, buffer_size, "MSVC %d", _MSC_FULL_VER);
#else
  name[0] = '\0';
#endif

if (L < 0){ // cppcheck-suppress knownConditionTrueFalse
  fprintf(stderr, "ERROR:ffilesystem:fs_compiler: snprintf failed\n");
  L = 0;
}

size_t N = (size_t) L;
if(N >= buffer_size){
  N = buffer_size-1;
  name[N] = '\0';
}

  return N;
}


size_t fs_longname(const char* in, char* out, size_t buffer_size)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea
// the path must exist
#ifdef _WIN32
// size includes null terminator
  DWORD Lb = (DWORD) buffer_size;
  DWORD L = GetLongPathNameA(in, out, Lb);
  if(L > 0)
    return L;

  fprintf(stderr, "ERROR:Ffs::longname:GetLongPathName: could not determine %s\n", in);
  return 0;
#else
  return fs_strncpy(in, out, buffer_size);
#endif
}


size_t fs_shortname(const char* in, char* out, size_t buffer_size)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getshortpathnamew
// the path must exist
#ifdef _WIN32
// size includes null terminator
  DWORD Lb = (DWORD) buffer_size;
  DWORD L = GetShortPathNameA(in, out, Lb);
  if(L > 0)
    return L;

  fprintf(stderr, "ERROR:Ffs::shortname:GetShortPathName: could not determine %s\n", in);
  return 0;
#else
  return fs_strncpy(in, out, buffer_size);
#endif
}

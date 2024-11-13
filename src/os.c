#include <stdbool.h>

#if defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
// https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#include <sys/param.h>
#endif


bool fs_is_android(){
#ifdef __ANDROID__
  return true;
#else
  return false;
#endif
}

bool fs_is_macos(){
// we don't use TargetConditionals.h because on macOS SDK updates sometimes including this header
// breaks compile until Homebrew / GCC updates.
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
#if defined(BSD)
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

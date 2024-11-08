#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include <cstdlib> // putenv, setenv, getenv

#include <string>
#include <string_view>

#ifdef _WIN32
#include <algorithm> // std::replace
#include <Objbase.h> // CoTaskMemFree
#include <KnownFolders.h> // FOLDERID_LocalAppData
#include <Shlobj.h> // SHGetKnownFolderPath
#endif

#include "ffilesystem.h"


std::string fs_getenv(std::string_view name)
{
  // don't emit error because sometimes we just check if envvar is defined

  #ifdef _MSC_VER
    size_t L;
    getenv_s(&L, nullptr, 0, name.data());
    if (L == 0)
      return {};

    std::string buf(L, '\0');
    getenv_s(&L, &buf[0], L, name.data());
    buf.resize(L - 1); // remove the null terminator
  #else
    auto buf = std::getenv(name.data());
    if(!buf)
      return {};
  #endif


  return buf;
}


bool fs_setenv(std::string_view name, std::string_view value)
{

#ifdef _WIN32
  // SetEnvironmentVariable returned OK but set blank values
  // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/putenv-wputenv

  if(_putenv_s(name.data(), value.data()) == 0)
    return true;
#else
  // https://www.man7.org/linux/man-pages/man3/setenv.3.html
  if(setenv(name.data(), value.data(), 1) == 0)
    return true;
#endif

  fs_print_error(name, "setenv");
  return false;
}

std::string fs_user_config_dir()
{
#ifdef _WIN32
  PWSTR s;
  if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &s) != S_OK) {
    CoTaskMemFree(s);
    return {};
  }

  std::wstring w(s);
  CoTaskMemFree(s);

  std::replace( w.begin(), w.end(), '\\', '/');
  return std::string(w.begin(), w.end());
#else
  std::string xdg = fs_getenv("XDG_CONFIG_HOME");
  if(!xdg.empty())
    return xdg;

  std::string home = fs_get_homedir();
  if(home.empty())
    return {};

  return home + "/" + ".config";
#endif
}

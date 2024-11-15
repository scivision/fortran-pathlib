#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <locale>

#if __has_include(<codecvt>)
#include <codecvt>
#endif

#include <string>
#include <string_view>

#include "ffilesystem.h"

std::string fs_get_locale_name() {
// https://en.cppreference.com/w/cpp/locale/locale/name
    return std::locale("").name();
}


std::string fs_to_narrow(std::wstring_view w)
{
  if(w.empty())
    return {};

  // https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
#ifdef _WIN32
  int L = WideCharToMultiByte(CP_UTF8, 0, w.data(), -1, nullptr, 0, nullptr, nullptr);
  if (L > 0) {
    std::string buf(L, '\0');
    L = WideCharToMultiByte(CP_UTF8, 0, w.data(), -1, buf.data(), L, nullptr, nullptr);

    if(L > 0){
      buf.resize(L-1);
      return buf;
    }
  }
#elif __has_include(<codecvt>)
  // deprecated in C++17, but no STL replacement
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(w.data());

#endif

  fs_print_error("", "fs_to_narrow");
  return {};
}

// test wide to narrow conversion with fs_to_narrow
// https://learn.microsoft.com/en-us/cpp/cpp/string-and-character-literals-cpp

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>

#include "ffilesystem.h"

int check_lang(std::wstring_view w, std::string_view n, std::string_view lang)
{
  const std::string r = fs_to_narrow(w);

  std::cout << lang << " lengths: input, reference, output: " << w.size() << " " << n.size()  << " " << r.size() << "\n";
  if (r != n){
    std::cout << "FAILED: fs_to_narrow() " << lang << " " << n << " != " << r << "\n";
    return 1;
  }

  std::cout << "PASSED: wide " << lang << " " << n << "\n";
  return 0;
}

int main()
{
  constexpr std::size_t Lw = sizeof(wchar_t);
  std::cout << "wchar_t size (bytes): " << Lw << "\n";

  int err = 0;

  err += check_lang(L"Hello, world!", "Hello, world!", "English");

  err += check_lang(L"¡Hola, mundo!", "¡Hola, mundo!", "Spanish");

  err += check_lang(L"你好，世界！", "你好，世界！", "Chinese");

  err += check_lang(L"こんにちは、世界！", "こんにちは、世界！", "Japanese");

  if(err)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

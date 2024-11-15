// test wide to narrow conversion with fs_to_narrow
// https://learn.microsoft.com/en-us/cpp/cpp/string-and-character-literals-cpp

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>

#include "ffilesystem.h"

int main()
{
  constexpr std::size_t Lw = sizeof(wchar_t);
  std::cout << "wchar_t size (bytes): " << Lw << "\n";

  std::wstring w = L"Hello, world!";
  std::string n = "Hello, world!";

  int err = 0;

  std::string r = fs_to_narrow(w);
  std::cout << "English lengths: input, reference, output: " << w.size() << " " << n.size()  << " " << r.size() << "\n";
  if (r != n){
    std::cout << "FAILED: fs_to_narrow() English " << n << " != " << r << "\n";
    err++;
  } else
    std::cout << "PASSED: wide English " << n << "\n";

  // Chinese
  w = L"你好，世界！";
  n = "你好，世界！";
  r = fs_to_narrow(w);
  std::cout << "Chinese lengths: input, reference, output: " << w.size() << " " << n.size()  << " " << r.size() << "\n";
  if (r != n){
    std::cerr << "FAILED: fs_to_narrow() Chinese " << n << " != " << r << "\n";
    err++;
  } else
    std::cout << "PASSED: wide Chinese " << n << "\n";

  // Japanese
  w = L"こんにちは、世界！";
  n = "こんにちは、世界！";
  r = fs_to_narrow(w);
  std::cout << "Japanese lengths: input, reference, output: " << w.size() << " " << n.size()  << " " << r.size() << "\n";
  if (r != n){
    std::cerr << "FAILED: fs_to_narrow() Japanese " << n << " != " << r  << "\n";
    err++;
  } else
    std::cout << "PASSED: wide Japanese " << n << "\n";


  if(err)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

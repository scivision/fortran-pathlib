#if __has_include(<version>)
#include <version>
#elif __has_include(<ciso644>)
// < C++20 standard
#include <ciso646>
#endif

#include <cstdlib>
#include <iostream>

unsigned long libcxx_release()
{
#if defined(_LIBCPP_VERSION)  // LLVM libc++
  return _LIBCPP_VERSION;
#elif defined(_GLIBCXX_RELEASE)  // GNU libstdc++
  return _GLIBCXX_RELEASE;
#else
  return 0;
#endif
}


int main(){

#if defined(_LIBCPP_VERSION)
  std::cout << "LLVM ";
#elif defined(_GLIBCXX_RELEASE)
  std::cout << "GNU ";
#endif

  std::cout << libcxx_release() << "\n";

  return EXIT_SUCCESS;
}

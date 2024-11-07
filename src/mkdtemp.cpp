#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "ffilesystem.h"

#include <iostream>
#include <algorithm>            // for generate_n
#include <array>                // for array
#include <functional>           // for ref
#include <iterator>             // for begin, end
#include <string>
#include <system_error>         // for error_code

#if defined(HAVE_CXX_FILESYSTEM)
#include <random>
#else
#ifdef _WIN32
#include <io.h>  // _mktemp
#else
#include <unistd.h> // mkdtemp macOS
#include <stdlib.h> // mkdtemp Linux
#endif
#endif


#if defined(HAVE_CXX_FILESYSTEM) && defined(__cpp_deduction_guides)
// CTAD C++17 random string generator
// https://stackoverflow.com/a/444614
// https://en.cppreference.com/w/cpp/language/class_template_argument_deduction

template <typename T = std::mt19937>
static auto fs_random_generator() -> T {
    auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
    auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
    auto seed = std::array<std::seed_seq::result_type, seed_len>();
    auto dev = std::random_device();
    std::generate_n(std::begin(seed), seed_len, std::ref(dev));
    auto seed_seq = std::seed_seq(std::begin(seed), std::end(seed));
    return T{seed_seq};
}


static std::string fs_generate_random_alphanumeric_string(const std::string::size_type len)
{
    constexpr std::string_view chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    thread_local auto rng = fs_random_generator<>();
    auto dist = std::uniform_int_distribution{{}, chars.length() - 1};
    auto result = std::string(len, '\0');
    std::generate_n(std::begin(result), len, [&]() { return chars[dist(rng)]; });
    return result;
}
#endif


std::string fs_mkdtemp(std::string_view prefix)
{
  // make unique temporary directory starting with prefix

if(FS_TRACE) std::cout << "TRACE:mkdtemp_mersenne: prefix: " << prefix << "\n";

  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM) && defined(__cpp_deduction_guides)

  std::filesystem::path t;

  constexpr std::string::size_type Lname = 16;  // arbitrary length for random string
  const std::filesystem::path temp = std::filesystem::temp_directory_path(ec);

  if(FS_TRACE) std::cout << "TRACE:mkdtemp_mersenne: tempdir: " << temp << "\n";

  if(!ec) FFS_LIKELY
  {
    do {
      const std::string rname = fs_generate_random_alphanumeric_string(Lname);
      t = (temp / (prefix.data() + rname));
      if(FS_TRACE) std::cout << "TRACE:mkdtemp_mersenne: randomName: " << rname << "\n";
      if(FS_TRACE) std::cout << "TRACE:mkdtemp_mersenne: fullTemppath: " << t << "\n";
    } while (std::filesystem::is_directory(t, ec) && !ec);

    if (std::filesystem::create_directory(t, ec) && !ec) FFS_LIKELY
      return t.generic_string();
  }

#else

  std::string tmpl(prefix);
  const char* tmp;

#ifdef _WIN32
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/mktemp-wmktemp
  tmp = _mktemp(tmpl.data());
  if(tmp)
    return fs_mkdir(tmp) ? tmp : std::string{};
#else
  // https://www.man7.org/linux/man-pages/man3/mkdtemp.3.html
  // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/mkdtemp.3.html

  tmp = mkdtemp(tmpl.data());
  if(tmp)
    return tmp;
#endif
#endif

  fs_print_error(prefix, "mkdtemp", ec);
  return {};
}

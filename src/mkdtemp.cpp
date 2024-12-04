#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include "ffilesystem.h"

#include <iostream>

#include <string>
#include <string_view>

#include <system_error>         // for error_code

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#ifdef _WIN32
#include <io.h>  // IWYU pragma: keep
// _mktemp_s
#else
#include <unistd.h> // mkdtemp macOS
#include <cstdlib> // mkdtemp Linux
#endif
#endif


std::string fs_mkdtemp(std::string_view prefix)
{
  // make unique temporary directory starting with prefix

if(fs_trace) std::cout << "TRACE:mkdtemp_mersenne: prefix: " << prefix << "\n";

  std::error_code ec;

#if defined(HAVE_CXX_FILESYSTEM)

  std::filesystem::path t;

  constexpr std::string::size_type Lname = 16;  // arbitrary length for random string
  const std::filesystem::path temp = std::filesystem::temp_directory_path(ec);

  if(fs_trace) std::cout << "TRACE:mkdtemp: tempdir: " << temp << "\n";

  if(!ec) FFS_LIKELY
  {
    do {
      const std::string rname = fs_generate_random_alphanumeric_string(Lname);
      if(rname.empty()) FFS_UNLIKELY
        return {};
      t = (temp / (prefix.data() + rname));
      if(fs_trace) std::cout << "TRACE:mkdtemp: randomName: " << rname << "\n";
      if(fs_trace) std::cout << "TRACE:mkdtemp: fullTemppath: " << t << "\n";
    } while (std::filesystem::is_directory(t, ec) && !ec);

    if (std::filesystem::create_directory(t, ec) && !ec) FFS_LIKELY
      return t.generic_string();
  }

#else

  std::string tmpl(prefix);
  const char* tmp;

#ifdef _WIN32
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/mktemp-wmktemp
  if (_mktemp_s(tmpl.data(), tmpl.size() + 1) == 0)
    tmp = tmpl.c_str();
  else
    tmp = nullptr;
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

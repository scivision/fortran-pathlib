#include <string>
#include <string_view>

#if __has_include(<ranges>)
#include <ranges>  // IWYU pragma: keep
#endif

#include <algorithm> // toupper, std::ranges::contains, std::find
#include <cctype> // std::isalnum
#include <vector>

#include "ffilesystem.h"


bool fs_is_reserved(std::string_view path)
{
  if(!fs_is_windows())
    return false;

  std::string s = fs_stem(path);
  auto L = s.length();
  if(L < 3 || L > 4)
    return false;

  // convert to upper case
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);

// check if the stem is a reserved device name
  const std::vector<std::string_view> r = {"CON", "PRN", "AUX", "NUL",
    "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
    "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};

#ifdef _cpp_lib_ranges_contains  // C++23
  if (std::ranges::contains(r, s))
#elif __cpp_lib_ranges // C++20
  if (std::ranges::find(r, s) != r.end())
#else // C++17
  if (std::find(r.begin(), r.end(), s) != r.end())
#endif
    return true;

return false;
}


static bool fs_is_safe_char(const char c)
{
  // unordered_set<char>  8us
  // set<char, std::less<>>  6us
  // vector<char> 0.3us so much faster!
  const std::vector<char> safe {'_', '-', '.', '~', '@', '#', '$', '%', '^', '&', '(', ')', '[', ']', '{', '}', '+', '=', ',', '!'};

  return std::isalnum(c) ||
#ifdef __cpp_lib_ranges // C++20
    std::ranges::find(safe, c)
#else // C++17
    std::find(safe.begin(), safe.end(), c)
#endif
    != safe.end();
}


bool fs_is_safe_name(std::string_view filename)
{
  // check that only shell-safe characters are in filename
  // hasn't been fully tested yet across operating systems and file systems--let us know if character(s) should be unsafe
  // does NOT check for reserved or device names
  // => filenames only, not paths
  // https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
  // we do not consider whitespaces, quotes, or ticks safe, as they can be confusing in shell scripts and command line usage

  if(fs_is_windows() && !filename.empty() && filename.back() == '.') FFS_UNLIKELY
    return false;

#ifdef __cpp_lib_ranges // C++20
  return std::ranges::all_of(filename, fs_is_safe_char);
#else // C++11
  return std::all_of(filename.begin(), filename.end(), fs_is_safe_char);
#endif
}

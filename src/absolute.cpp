#include <string>
#include <string_view>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#include <system_error>
#endif

#include "ffilesystem.h"


std::string fs_absolute(std::string_view path, const bool expand_tilde)
{
  // wraps std::filesystem::absolute(path).
  // path need not exist
  // Inspired by Python pathlib.Path.absolute()
  // https://docs.python.org/3/library/pathlib.html#pathlib.Path.absolute

  const std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);

  // Linux, MinGW can't handle empty paths
  if(ex.empty())
    return fs_get_cwd();

  if (fs_is_absolute(ex))
    return ex;

#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;

  const auto a = std::filesystem::absolute(ex, ec);

  if(!ec) FFS_LIKELY
    return a.generic_string();

  fs_print_error(path, "absolute", ec);
  return {};
#else
  std::string cwd = fs_get_cwd();
  if(cwd.empty())
    return {};

  if(cwd.back() != '/')
    cwd += '/';

  return cwd + ex;
#endif
}



std::string fs_absolute(std::string_view path, std::string_view base, const bool expand_tilde)
{
  // rebase path on base.

  const std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);

  if(fs_is_absolute(ex))
    return ex;

  std::string b = fs_absolute(base, expand_tilde);
  if(b.empty())
    return {};

  if(!ex.empty() && b.back() != '/')
    b += '/';

  // don't need join(). Keeps it like Python pathlib.Path.absolute()
  return b + ex;
}

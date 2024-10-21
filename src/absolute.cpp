#include <string>
#include <iostream>
#include <system_error>

#include "ffilesystem.h"


std::string fs_absolute(std::string_view path, const bool expand_tilde)
{
  // wraps std::filesystem::absolute(path).
  // path need not exist
  // Inspired by Python pathlib.Path.absolute()
  // https://docs.python.org/3/library/pathlib.html#pathlib.Path.absolute

#ifdef HAVE_CXX_FILESYSTEM
  const std::filesystem::path ex = expand_tilde
    ? std::filesystem::path(fs_expanduser(path))
    : path;

  if (ex.is_absolute())
    return ex.generic_string();
#else
  const std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);
#endif

  // Linux, MinGW can't handle empty paths
  if(ex.empty())
    return fs_get_cwd().value_or("");

#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;

  const auto a = std::filesystem::absolute(ex, ec);

  if(!ec) FFS_LIKELY
    return a.generic_string();

  std::cerr << "ERROR:Ffilesystem:absolute(" << path << ") " << ec.message() << "\n";
  return {};
#else
  if (fs_is_absolute(ex))
    return ex;

  const auto cwd = fs_get_cwd();
  if(!cwd)
    return {};

  return (cwd.value().back() != '/')
    ? cwd.value() + '/' + ex
    : cwd.value() + ex;
#endif
}



std::string fs_absolute(std::string_view path, std::string_view base, const bool expand_tilde)
{
  // rebase path on base.

#ifdef HAVE_CXX_FILESYSTEM
  const std::filesystem::path ex = expand_tilde
    ? std::filesystem::path(fs_expanduser(path))
    : path;

  if (ex.is_absolute())
    return ex.generic_string();
#else
  const std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);

  if(fs_is_absolute(ex))
    return ex;
#endif

  std::string b = fs_absolute(base, expand_tilde);
  if(b.empty())
    return {};

  if(!ex.empty() && b.back() != '/')
    b += '/';

  // don't need join(). Keeps it like Python pathlib.Path.absolute()
  b +=
#ifdef HAVE_CXX_FILESYSTEM
    ex.generic_string();
#else
    ex;
#endif

  return b;
}
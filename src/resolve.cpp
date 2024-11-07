#include "ffilesystem.h"

#include <iostream>
#include <string>
#include <system_error>


std::string fs_canonical(std::string_view path, const bool strict, const bool expand_tilde)
{
  // canonicalize path, i.e. resolve all symbolic links, remove ".", ".." and extra slashes
  // if strict is true, then path must exist

  if (path.empty()) FFS_UNLIKELY
    return {};
    // need this for macOS otherwise it returns the current working directory instead of empty string

  const std::string ex = expand_tilde
    ? fs_expanduser(path)
    : std::string(path);

#ifdef HAVE_CXX_FILESYSTEM

  // handle differences in ill-defined behaviour of std::filesystem::weakly_canonical() on non-existent paths
  // canonical(path, false) is distinct from resolve(path, false) for non-existing paths.
  // don't just always normalize because canonical also resolve symlinks.
  if (!strict && !fs_is_absolute(ex) && !fs_exists(ex))
    return fs_normal(ex);

  if (fs_is_mingw() && fs_is_symlink(ex))
    return fs_win32_final_path(ex);

  std::error_code ec;

  const auto c = strict
    ? std::filesystem::canonical(ex, ec)
    : std::filesystem::weakly_canonical(ex, ec);

  if(!ec) FFS_LIKELY
    return c.generic_string();

  fs_print_error(path, "canonical", ec);
  return {};
#else

  // non c++ filesystem uses different strict logic
  if (!fs_exists(ex)) {
    if (strict) {
      std::cerr << "ERROR:ffilesystem:canonical(" << path << ") path does not exist and strict=true\n";
      return {};
    }
    return fs_normal(ex);
  }

  return fs_realpath(ex);

#endif
}


std::string fs_resolve(std::string_view path, const bool strict, const bool expand_tilde)
{
  // works like canonical(absolute(path)).
  // Inspired by Python pathlib.Path.resolve()
  // https://docs.python.org/3/library/pathlib.html#pathlib.Path.resolve

  return fs_canonical(fs_absolute(path, expand_tilde), strict, false);
}

#include "ffilesystem.h"

#include <iostream>
#include <string>
#include <system_error>

#ifndef HAVE_CXX_FILESYSTEM
// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#endif


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

  std::error_code ec;

  const auto c = strict
    ? std::filesystem::canonical(ex, ec)
    : std::filesystem::weakly_canonical(ex, ec);

  if(!ec) FFS_LIKELY
    return c.generic_string();

  std::cerr << "ERROR:ffilesystem:canonical(" << path << ") " << ec.message() << "\n";
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


bool fs_equivalent(std::string_view path1, std::string_view path2)
{
  // non-existent paths are not equivalent
#ifdef HAVE_CXX_FILESYSTEM
  std::error_code ec;
  std::filesystem::path p1(path1);
  std::filesystem::path p2(path2);
  if(fs_is_mingw()){
    p1 = p1.lexically_normal();
    p2 = p2.lexically_normal();
  }

  bool e = std::filesystem::equivalent(p1, p2, ec);
  if(ec){
    std::cerr << "ERROR:ffilesystem:equivalent(" << path1 << ", " << path2 << ") " << ec.message() << "\n";
    e = false;
  }

  return e;
#else
  struct stat s1;
  struct stat s2;

  if(stat(path1.data(), &s1) != 0 || stat(path2.data(), &s2) != 0)
    return false;
// https://www.boost.org/doc/libs/1_85_0/libs/filesystem/doc/reference.html#equivalent
  return s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino;
#endif
}

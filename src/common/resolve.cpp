#include "ffilesystem.h"

#include <iostream>
#include <string>
#include <system_error>


std::optional<std::string> Ffs::canonical(std::string_view path, const bool strict, const bool expand_tilde)
{
  // canonicalizes path, which need not exist

  if (path.empty()) FFS_UNLIKELY
    return {};
    // need this for macOS otherwise it returns the current working directory instead of empty string

  const auto ex = expand_tilde
    ? std::filesystem::path(fs_expanduser(path))
    : path;

  std::error_code ec;

  if (!strict && !ex.is_absolute() && (!std::filesystem::exists(ex, ec) || ec)){
    // handles differences in ill-defined behaviour of std::filesystem::weakly_canonical() on non-existent paths
    // canonical(path, false) is distinct from resolve(path, false) for non-existing paths.
    return Ffs::normal(ex.generic_string());
  }

  const auto c = strict
    ? std::filesystem::canonical(ex, ec)
    : std::filesystem::weakly_canonical(ex, ec);

  if(!ec) FFS_LIKELY
    return c.generic_string();

  std::cerr << "ERROR:ffilesystem:canonical(" << path << ") " << ec.message() << "\n";
  return {};
}


std::optional<std::string> Ffs::resolve(std::string_view path, const bool strict, const bool expand_tilde)
{
  // works like canonical(absolute(path)).
  // Inspired by Python pathlib.Path.resolve()
  // https://docs.python.org/3/library/pathlib.html#pathlib.Path.resolve

  auto a = fs_absolute(path, expand_tilde);
  if (a.empty()) FFS_UNLIKELY
    return {};

  return Ffs::canonical(a, strict, false);
}


bool Ffs::equivalent(std::string_view path1, std::string_view path2)
{
  // non-existent paths are not equivalent
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
}

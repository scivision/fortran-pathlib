#include "ffilesystem.h"

#include <iostream>


std::string Ffs::canonical(std::string_view path, const bool strict)
{
  // also expands ~ and normalizes path

  if (path.empty()) FFS_UNLIKELY
    return {};
    // need this for macOS otherwise it returns the current working directory instead of empty string

  auto ex = std::filesystem::path(Ffs::expanduser(path));

  std::error_code ec;

  if (!strict && !ex.is_absolute() && (!std::filesystem::exists(ex, ec) || ec)){
    // handles differences in ill-defined behaviour of std::filesystem::weakly_canonical() on non-existent paths
    // canonical(path, false) is distinct from resolve(path, false) for non-existing paths.
    return Ffs::normal(ex.generic_string());
  }

  auto c = strict
    ? std::filesystem::canonical(ex, ec)
    : std::filesystem::weakly_canonical(ex, ec);

  if(!ec) FFS_LIKELY
    return c.generic_string();

  std::cerr << "ERROR:ffilesystem:canonical(" << path << ") " << ec.message() << "\n";
  return {};
}


std::string Ffs::resolve(std::string_view path, const bool strict)
{
  // expands ~ like canonical
  // empty path returns current working directory, which is distinct from canonical that returns empty string
  if(path.empty()) FFS_UNLIKELY
    return Ffs::get_cwd();

  auto ex = std::filesystem::path(Ffs::expanduser(path));

  std::error_code ec;

  auto c = strict
    ? std::filesystem::canonical(ex, ec)
    : std::filesystem::weakly_canonical(ex, ec);

  if(!ec) FFS_LIKELY
    return c.generic_string();

  std::cerr << "ERROR:ffilesystem:resolve(" << path << ") " << ec.message() << "\n";
  return {};
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
  return std::filesystem::equivalent(p1, p2, ec) && !ec;
}


std::string Ffs::which(std::string_view name)
// find full path to executable name on Path
{
  std::string n(name);

  if (Ffs::is_exe(n))
    return Ffs::as_posix(n);

  // relative directory component, but path was not a file
  if(Ffs::file_name(n) != n)
    return {};

  // Windows gives priority to cwd, so check that first
  if(fs_is_windows()){
    if(std::string t = Ffs::get_cwd() + "/" + n; Ffs::is_exe(t))
      return Ffs::as_posix(t);
  }
  const char pathsep = fs_pathsep();

  std::string path = Ffs::get_env("PATH");
  if (path.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:which: Path environment variable not set\n";
    return {};
  }

  std::string::size_type start = 0;
  std::string::size_type end = path.find_first_of(pathsep, start);
  std::string p;

  while (end != std::string::npos) {
    p = path.substr(start, end - start) + "/" + n;
    if (FS_TRACE) std::cout << "TRACE:ffilesystem:which: " << p << "\n";
    if (Ffs::is_exe(p))
      return Ffs::as_posix(p);

    start = end + 1;
    end = path.find_first_of(pathsep, start);
  }

  p = path.substr(start) + "/" + n;
  if(Ffs::is_exe(p))
    return Ffs::as_posix(p);

  return {};
}


std::string Ffs::make_absolute(std::string_view path, std::string_view base)
{
  std::string out = Ffs::expanduser(path);

  if (Ffs::is_absolute(out))
    return out;

  return Ffs::join(Ffs::expanduser(base), out);
}

#include "ffilesystem.h"

#include <cstddef> // size_t
#include <iostream>

#include <filesystem>

size_t fs_canonical(const char* path, bool strict, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::canonical(std::string_view(path), strict), result, buffer_size);
}

std::string Ffs::canonical(std::string_view path, bool strict)
{
  // also expands ~ and normalizes path

  if (path.empty()) FFS_UNLIKELY
    return {};
    // need this for macOS otherwise it returns the current working directory instead of empty string

  auto ex = Ffs::expanduser(path);

  if(FS_TRACE) std::cout << "TRACE:canonical: input: " << path << " expanded: " << ex << "\n";

  if (!Ffs::exists(ex) && !Ffs::is_absolute(ex))
    // handles differences in ill-defined behaviour of std::filesystem::weakly_canonical() on non-existant paths
    // canonical(path, false) is distinct from resolve(path, false) for non-existing paths.
    return Ffs::normal(ex);

  std::error_code ec;
  auto c = strict
    ? std::filesystem::canonical(ex, ec)
    : std::filesystem::weakly_canonical(ex, ec);

  if(!ec) FFS_LIKELY
    return c.generic_string();

  std::cerr << "ERROR:ffilesystem:canonical: " << ec.message() << "\n";
  return {};
}


size_t fs_resolve(const char* path, bool strict, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::resolve(std::string_view(path), strict), result, buffer_size);
}

std::string Ffs::resolve(std::string_view path, bool strict)
{
  // expands ~ like canonical
  // empty path returns current working directory, which is distinct from canonical that returns empty string
  if(path.empty()) FFS_UNLIKELY
    return Ffs::get_cwd();

  auto ex = Ffs::expanduser(path);

  if (!Ffs::exists(ex) && !Ffs::is_absolute(ex))
    // handles differences in ill-defined behaviour of std::filesystem::weakly_canonical() on non-existant paths
    // canonical(path, false) is distinct from resolve(path, false) for non-existing paths.
    ex = Ffs::join(Ffs::get_cwd(), ex);

  std::error_code ec;
  auto c = strict
    ? std::filesystem::canonical(ex, ec)
    : std::filesystem::weakly_canonical(ex, ec);

  if(!ec) FFS_LIKELY
    return c.generic_string();

  std::cerr << "ERROR:ffilesystem:resolve: " << ec.message() << "\n";
  return {};
}


bool fs_equivalent(const char* path1, const char* path2)
{
  return Ffs::equivalent(std::string_view(path1), std::string_view(path2));
}

bool Ffs::equivalent(std::string_view path1, std::string_view path2)
{
  // non-existant paths are not equivalent
  std::error_code ec;
  return std::filesystem::equivalent(Ffs::expanduser(path1), Ffs::expanduser(path2), ec) && !ec;
}


size_t fs_relative_to(const char* base, const char* other, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::relative_to(base, other), result, buffer_size);
}

std::string Ffs::relative_to(std::string_view base, std::string_view other)
{
  // std::filesystem::relative resolves symlinks and normalizes both paths first

  // undefined case, avoid bugs with MacOS
  if (base.empty() || other.empty()) FFS_UNLIKELY
    return {};

  std::filesystem::path basep(base);
  std::filesystem::path otherp(other);
  // cannot be relative, avoid bugs with MacOS
  if(basep.is_absolute() != otherp.is_absolute())
    return {};

  std::error_code ec;
  if (auto r = std::filesystem::relative(otherp, basep, ec); !ec) FFS_LIKELY
    return r.generic_string();

  std::cerr << "ERROR:ffilesystem:relative_to: " << ec.message() << "\n";
  return {};
}


size_t fs_proximate_to(const char* base, const char* other, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::proximate_to(base, other), result, buffer_size);
}

std::string Ffs::proximate_to(std::string_view base, std::string_view other)
{
  // std::filesystem::proximate resolves symlinks and normalizes both paths first

  // undefined case, avoid bugs with MacOS
  if(other.empty())
    return {};

  // undefined case, avoid bugs with MacOS and Windows
  if (base.empty())
      return std::string(other);

  std::filesystem::path basep(base);
  std::filesystem::path otherp(other);
  // cannot be relative, avoid bugs with AppleClang
  if(basep.is_absolute() != otherp.is_absolute())
    return otherp.generic_string();

  std::error_code ec;
  if(auto r = std::filesystem::proximate(otherp, basep, ec); !ec) FFS_LIKELY
    return r.generic_string();

  std::cerr << "ERROR:ffilesystem:proximate_to: " << ec.message() << "\n";
  return {};
}


size_t fs_which(const char* name, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::which(std::string_view(name)), result, buffer_size);
}

std::string Ffs::which(std::string_view name)
// find full path to executable name on Path
{
  std::string n(name);

  if (Ffs::is_absolute(n))
    return Ffs::is_exe(n) ? n : std::string();

  const char pathsep = fs_pathsep();

  std::string path = Ffs::get_env("PATH");
  if (path.empty()) FFS_UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:which: Path environment variable not set\n";
    return {};
  }

  // Windows gives priority to cwd, so check that first
  if(fs_is_windows())
    path = Ffs::get_cwd() + pathsep + path;

  std::string::size_type start = 0;
  std::string::size_type end = path.find_first_of(pathsep, start);
  std::string p;

  while (end != std::string::npos) {
    p = path.substr(start, end - start) + "/" + n;
    if (FS_TRACE) std::cout << "TRACE:ffilesystem:which: " << p << "\n";
    if (Ffs::is_exe(p))
      return p;

    start = end + 1;
    end = path.find_first_of(pathsep, start);
  }

  p = path.substr(start) + "/" + n;
  if(Ffs::is_exe(p))
    return p;

  return {};
}


bool fs_is_subdir(const char* subdir, const char* dir)
{
  return Ffs::is_subdir(std::string_view(subdir), std::string_view(dir));
}

bool Ffs::is_subdir(std::string_view subdir, std::string_view dir)
{
  auto r = std::filesystem::relative(subdir, dir);

  return !r.empty() && r.native().front() != '.';
}



size_t fs_make_absolute(const char* path, const char* base, char* out, size_t buffer_size)
{
  return fs_str2char(Ffs::make_absolute(std::string_view(path), std::string_view(base)), out, buffer_size);
}

std::string Ffs::make_absolute(std::string_view path, std::string_view base)
{
  std::string out = Ffs::expanduser(path);

  if (Ffs::is_absolute(out))
    return out;

  return Ffs::join(Ffs::expanduser(base), out);
}

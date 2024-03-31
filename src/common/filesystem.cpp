// functions from C++ filesystem

// NOTE: this segfaults: std::filesystem::path p(nullptr);


#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#endif

// GCC itself does it this way https://github.com/gcc-mirror/gcc/blob/78b56a12dd028b9b4051422c6bad6260055e4465/libcpp/system.h#L426
#ifdef __has_cpp_attribute
#if __has_cpp_attribute(unlikely)
#define UNLIKELY [[unlikely]]
#define LIKELY [[likely]]
#endif
#endif
#ifndef UNLIKELY
#define UNLIKELY
#define LIKELY
#endif

#include <iostream>
#include <algorithm>
#include <array>
#include <memory> // std::make_unique
#include <functional>
#include <random>
#include <cstring>
#include <string>
#include <fstream>
#include <set>
#include <cstdint>
#include <cstdlib>
#include <system_error>
#include <cctype> // std::isalnum
#include <filesystem>

#if __has_include(<format>)
#include <format>
#endif

#include "ffilesystem.h"

// for get_homedir backup method
#ifdef _WIN32
#include <UserEnv.h>
#else
#include <sys/types.h>
#include <pwd.h>
#include <cerrno>
#include <unistd.h> // for mac too
#endif
// end get_homedir backup method

// for lib_path, exe_path
#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(HAVE_DLADDR)
#include <dlfcn.h>
static void dl_dummy_func() {}
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#elif defined(__linux__) || defined(__CYGWIN__)
#include <unistd.h>
#endif
// --- end of lib_path, exe_path

#if __has_include(<sys/utsname.h>)
#include <sys/utsname.h>
#endif

static std::string fs_generate_random_alphanumeric_string(std::size_t);


size_t fs_get_max_path(){

  size_t m = 256;
#if defined(PATH_MAX)
  m = PATH_MAX;
#elif defined (_MAX_PATH)
  m = _MAX_PATH;
#elif defined (_POSIX_PATH_MAX)
  m = _POSIX_PATH_MAX;
#endif
  return (m < 4096) ? m : 4096; // arbitrary absolute maximum

}


bool fs_cpp()
{
// tell if fs core is C or C++
  return true;
}

long fs_lang()
{
  return __cplusplus;
}


int fs_is_wsl() {
#if __has_include(<sys/utsname.h>)
  struct utsname buf;
  if (uname(&buf) != 0) UNLIKELY
    return false;

  std::string_view release(buf.release);

  if (std::string_view(buf.sysname) != "Linux") UNLIKELY
    return 0;

#ifdef __cpp_lib_starts_ends_with
    if (release.ends_with("microsoft-standard-WSL2"))
      return 2;
    if (release.ends_with("-Microsoft"))
      return 1;
#endif
    return -1;
#else
  return 0;
#endif
}

static size_t fs_str2char(std::string_view s, char* result, size_t buffer_size)
{
  if(s.length() >= buffer_size) UNLIKELY
  {
    result = nullptr;
    std::cerr << "ERROR:ffilesystem: output buffer " << buffer_size << " too small for string: " << s << "\n";
    return 0;
  }

  s.copy(result, buffer_size);
  result[s.length()] = '\0';
  return s.length();
}


static bool fs_is_safe_char(const char c)
{
  std::set<char, std::less<>> safe = {'_', '-', '.', '~', '@', '#', '$', '%', '^', '&', '(', ')', '[', ']', '{', '}', '+', '=', ',', '!'};

  return std::isalnum(c) ||
#if __cplusplus >= 202002L
    safe.contains(c);
#else
    safe.find(c) != safe.end();
#endif

}

bool fs_is_safe_name(const char* filename)
{
  return Ffs::is_safe_name(std::string_view(filename));
}

bool Ffs::is_safe_name(std::string_view filename)
{
  // check that only shell-safe characters are in filename using std::isalnum and a c++ set
  // hasn't been fully tested yet across operating systems and file systems--let us know if character(s) should be unsafe
  // does NOT check for reserved or device names
  // => filenames only, not paths
  // https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
  // we do not consider whitespaces, quotes, or ticks safe, as they can be confusing in shell scripts and command line usage

  if(filename.empty()) UNLIKELY
    return false;

  if(fs_is_windows() && filename.back() == '.') UNLIKELY
    return false;

#ifdef __cpp_lib_ranges
  return std::ranges::all_of(filename, fs_is_safe_char);
#else
  return std::all_of(filename.begin(), filename.end(), fs_is_safe_char);
#endif
}


std::string Ffs::compiler()
{
#ifdef __cpp_lib_format

#if defined(__INTEL_LLVM_COMPILER)
  return std::format("Intel LLVM {} {}", __INTEL_LLVM_COMPILER,  __VERSION__);
#elif defined(__NVCOMPILER_LLVM__)
  return std::format("NVIDIA nvc {}.{}.{}", __NVCOMPILER_MAJOR__, __NVCOMPILER_MINOR__, __NVCOMPILER_PATCHLEVEL__);
#elif defined(__clang__)
  #ifdef __VERSION__
    return std::format("Clang {}", __VERSION__);
  #else
    return std::format("Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
  #endif
#elif defined(__GNUC__)
  return std::format("GNU GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
  return std::format("MSVC {}", _MSC_FULL_VER);
#else
  return {};
#endif

#else
  return {};
#endif
}


void fs_as_posix(char* path)
{
  std::string p = Ffs::as_posix(std::string_view(path));
  fs_str2char(p, path, strlen(path)+1);
}

std::string Ffs::as_posix(std::string_view path)
{
  // force posix file separator on Windows
  return fs::path(path).generic_string();
}


size_t fs_normal(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::normal(std::string_view(path)), result, buffer_size);
}

std::string Ffs::normal(std::string_view path)
{
  std::string s = fs::path(path).lexically_normal().generic_string();
  // remove trailing slash
  if (s.length() > 1 && s.back() == '/')
    s.pop_back();
  return s;
}

std::string Ffs::lexically_normal(std::string_view path)
{
  return fs::path(path).lexically_normal().generic_string();
}

std::string Ffs::make_preferred(std::string_view path)
{
  return fs::path(path).make_preferred().generic_string();
}


size_t fs_file_name(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::file_name(std::string_view(path)), result, buffer_size);
}

std::string Ffs::file_name(std::string_view path)
{
  return fs::path(path).filename().generic_string();
}


size_t fs_stem(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::stem(std::string_view(path)), result, buffer_size);
}

std::string Ffs::stem(std::string_view path)
{
  return fs::path(path).filename().stem().generic_string();
}


size_t fs_join(const char* path, const char* other, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::join(std::string_view(path), std::string_view(other)), result, buffer_size);
}

std::string Ffs::join(std::string_view path, std::string_view other)
{
  if (other.empty()) UNLIKELY
    return std::string(path);

  return Ffs::normal((fs::path(path) / other).generic_string());
}


size_t fs_parent(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::parent(std::string_view(path)), result, buffer_size);
}

std::string Ffs::parent(std::string_view path)
{
  return fs::path(Ffs::normal(path)).parent_path().generic_string();
}


size_t fs_suffix(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::suffix(std::string_view(path)), result, buffer_size);
}

std::string Ffs::suffix(std::string_view path)
{
  return fs::path(path).filename().extension().generic_string();
}


size_t fs_with_suffix(const char* path, const char* new_suffix,
                      char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::with_suffix(std::string_view(path), std::string_view(new_suffix)), result, buffer_size);
}

std::string Ffs::with_suffix(std::string_view path, std::string_view new_suffix)
{
  return fs::path(path).replace_extension(new_suffix).generic_string();
}


bool fs_is_symlink(const char* path)
{
  return Ffs::is_symlink(std::string_view(path));
}

bool Ffs::is_symlink(std::string_view path)
{
  if (path.empty()) UNLIKELY
    return false;

#ifdef WIN32_SYMLINK
  DWORD a = GetFileAttributes(path.data());
  return (a != INVALID_FILE_ATTRIBUTES) && (a & FILE_ATTRIBUTE_REPARSE_POINT);
#else
  std::error_code ec;
  auto s = fs::symlink_status(path, ec);
  // NOTE: use of symlink_status here like lstat(), else logic is wrong with fs::status()

  return !ec && fs::is_symlink(s);
#endif
}


size_t fs_read_symlink(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::read_symlink(std::string_view(path)), result, buffer_size);
}

std::string Ffs::read_symlink(std::string_view path)
{
  std::error_code ec;
  auto p = fs::read_symlink(path, ec);
  if(ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:read_symlink: " << ec.message() << "\n";
    return {};
  }

  return p.generic_string();
  // fs::canonical fallback not helpful here
}

bool fs_create_symlink(const char* target, const char* link)
{
  try{
    return Ffs::create_symlink(std::string_view(target), std::string_view(link));
  } catch(fs::filesystem_error& e){
    std::cerr << "ERROR:ffilesystem:create_symlink: " << e.what() << "\n";
    return false;
  }
}

bool Ffs::create_symlink(std::string_view target, std::string_view link)
{
  if(target.empty()) UNLIKELY
  {
    std::cerr << "ERROR: Ffs::create_symlink: target path must not be empty\n";
    // confusing program errors if target is "" -- we'd never make such a symlink in real use.
    return false;
  }
  if(link.empty()) UNLIKELY
  {
    std::cerr << "ERROR: Ffs::create_symlink: link path must not be empty\n";
    // macOS needs empty check to avoid SIGABRT
    return false;
  }

  auto s = fs::status(target);

#ifdef WIN32_SYMLINK
  DWORD p = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

  if(fs::is_directory(s))
    p |= SYMBOLIC_LINK_FLAG_DIRECTORY;

  if (CreateSymbolicLink(link.data(), target.data(), p)) LIKELY
    return true;

  DWORD err = GetLastError();
  std::string msg = "ERROR:Ffs:CreateSymbolicLink";
  if(err == ERROR_PRIVILEGE_NOT_HELD)
    msg += "Enable Windows developer mode to use symbolic links: https://learn.microsoft.com/en-us/windows/apps/get-started/developer-mode-features-and-debugging";

  std::cerr << msg << ": " << link << " => " << target << "\n";
  return false;
#else
  fs::is_directory(s)
    ? fs::create_directory_symlink(target, link)
    : fs::create_symlink(target, link);

  return true;
#endif
}

bool fs_mkdir(const char* path)
{
  return Ffs::mkdir(std::string_view(path));
}

bool Ffs::mkdir(std::string_view path)
{

  fs::path p(path);

  std::error_code ec;
  auto s = fs::is_directory(p, ec);
  if (!ec && s)
    return true;

  fs::create_directories(p, ec);
  // old MacOS return false even if directory was created
  if (ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:mkdir: " << ec.message() << "\n";
    return false;
  }

  s = fs::is_directory(p, ec);
  return !ec && s;
}


size_t fs_root(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::root(std::string_view(path)), result, buffer_size);
}

std::string Ffs::root(std::string_view path)
{
  fs::path p(path);
  return p.root_path().generic_string();
}


bool fs_exists(const char* path)
{
  return Ffs::exists(std::string_view(path));
}

bool Ffs::exists(std::string_view path)
{
  // fs
  std::error_code ec;
  auto s = fs::status(path, ec);

  return !ec && fs::exists(s);
}


bool fs_is_absolute(const char* path)
{
  return Ffs::is_absolute(std::string_view(path));
}

bool Ffs::is_absolute(std::string_view path)
{
  fs::path p(path);
  return p.is_absolute();
}

bool fs_is_char_device(const char* path)
{
  // special POSIX file character device like /dev/null
  return Ffs::is_char_device(std::string_view(path));
}

bool Ffs::is_char_device(std::string_view path)
{
  std::error_code ec;
  auto s = fs::status(path, ec);
  return !ec && fs::is_character_file(s);
}


bool fs_is_dir(const char* path)
{
  return Ffs::is_dir(std::string_view(path));
}

bool Ffs::is_dir(std::string_view path)
{
  fs::path p(path);

  if (fs_is_windows() && !path.empty() && p.root_name() == p)
    return true;

  std::error_code ec;
  auto s = fs::status(p, ec);
  return !ec && fs::is_directory(s);
}


bool fs_is_exe(const char* path)
{
  return Ffs::is_exe(std::string_view(path));
}

bool Ffs::is_exe(std::string_view path)
{
  std::error_code ec;

  auto s = fs::status(path, ec);
  // need reserved check for Windows
  if(ec || !fs::is_regular_file(s) || Ffs::is_reserved(path))
    return false;

if(fs_is_mingw()){
  // Windows MinGW bug with executable bit
  return Ffs::is_readable(path);
}

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  fs::perms none = fs::perms::none;
  fs::perms others_exec = fs::perms::others_exec;
  fs::perms group_exec = fs::perms::group_exec;
  fs::perms owner_exec = fs::perms::owner_exec;
#endif

  auto i = s.permissions() & (owner_exec | group_exec | others_exec);
  return i != none;
}


bool fs_is_readable(const char* path)
{
  return Ffs::is_readable(std::string_view(path));
}

bool Ffs::is_readable(std::string_view path)
{
  std::error_code ec;
  auto s = fs::status(path, ec);
  if(ec || !fs::exists(s))
    return false;

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  fs::perms none = fs::perms::none;
  fs::perms owner_read = fs::perms::owner_read;
  fs::perms group_read = fs::perms::group_read;
  fs::perms others_read = fs::perms::others_read;
#endif

  auto i = s.permissions() & (owner_read | group_read | others_read);
  return i != none;
}


bool fs_is_writable(const char* path)
{
  return Ffs::is_writable(std::string_view(path));
}

bool Ffs::is_writable(std::string_view path)
{
  std::error_code ec;
  auto s = fs::status(path, ec);
  if(ec || !fs::exists(s))
    return false;

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  fs::perms owner_write = fs::perms::owner_write;
  fs::perms group_write = fs::perms::group_write;
  fs::perms others_write = fs::perms::others_write;
  fs::perms none = fs::perms::none;
#endif

  auto i = s.permissions() & (owner_write | group_write | others_write);
  return i != none;
}


bool fs_is_file(const char* path)
{
  return Ffs::is_file(std::string_view(path));
}

bool Ffs::is_file(std::string_view path)
{
  std::error_code ec;
  auto s = fs::status(path, ec);

  // disqualify reserved names
  return !ec && (fs::is_regular_file(s) && !Ffs::is_reserved(path));
}


bool fs_is_reserved(const char* path)
{
  return Ffs::is_reserved(std::string_view(path));
}

bool Ffs::is_reserved(std::string_view path)
// https://learn.microsoft.com/en-gb/windows/win32/fileio/naming-a-file#naming-conventions
{
  if (path.empty()) UNLIKELY
    return false;

#ifndef _WIN32
  return false;
#elif defined(__cpp_lib_ranges)

  std::set<std::string_view, std::less<>> reserved {
      "CON", "PRN", "AUX", "NUL",
      "COM0", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
      "LPT0", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};

  std::string s = Ffs::stem(path);

  std::ranges::transform(s.begin(), s.end(), s.begin(), ::toupper);

  return reserved.contains(s);

#else
  std::cout << "WARNING:ffilesystem:is_reserved: C++20 required for reserved names check\n";
  return false;
#endif
}


bool fs_remove(const char* path)
{
  return Ffs::remove(std::string_view(path));
}

bool Ffs::remove(std::string_view path)
{
  std::error_code ec;
  auto b = fs::remove(path, ec);
  if (ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:remove: " << ec.message() << "\n";
    return false;
  }
  return b;
}

size_t fs_canonical(const char* path, bool strict, char* result, size_t buffer_size)
{
  try{
    return fs_str2char(Ffs::canonical(std::string_view(path), strict), result, buffer_size);
  } catch(std::filesystem::filesystem_error& e){
    std::cerr << "ERROR:ffilesystem:canonical: " << e.what() << "\n";
    return 0;
  }
}

std::string Ffs::canonical(std::string_view path, bool strict)
{
  // also expands ~

  if (path.empty()) UNLIKELY
    return {};
    // need this for macOS otherwise it returns the current working directory instead of empty string

  auto ex = fs::path(Ffs::expanduser(path));

  if(FS_TRACE) std::cout << "TRACE:canonical: input: " << path << " expanded: " << ex << "\n";

  if (!fs::exists(ex) && !ex.is_absolute())
    // handles differences in ill-defined behaviour of fs::weakly_canonical() on non-existant paths
    // canonical(path, false) is distinct from resolve(path, false) for non-existing paths.
    return ex.generic_string();

  return strict
    ? fs::canonical(ex).generic_string()
    : fs::weakly_canonical(ex).generic_string();
}


size_t fs_resolve(const char* path, bool strict, char* result, size_t buffer_size)
{
  try{
    return fs_str2char(Ffs::resolve(std::string_view(path), strict), result, buffer_size);
  } catch(std::filesystem::filesystem_error& e){
    std::cerr << "ERROR:ffilesystem:resolve: " << e.what() << "\n";
    return 0;
  }
}

std::string Ffs::resolve(std::string_view path, bool strict)
{
  // expands ~ like canonical
  // empty path returns current working directory, which is distinct from canonical that returns empty string
  if(path.empty()) UNLIKELY
    return Ffs::get_cwd();

  auto ex = fs::path(Ffs::expanduser(path));

  if (!fs::exists(ex) && !ex.is_absolute())
    // handles differences in ill-defined behaviour of fs::weakly_canonical() on non-existant paths
    // canonical(path, false) is distinct from resolve(path, false) for non-existing paths.
    ex = Ffs::get_cwd() / ex;

  return strict
    ? fs::canonical(ex).generic_string()
    : fs::weakly_canonical(ex).generic_string();
}


bool fs_equivalent(const char* path1, const char* path2)
{
  return Ffs::equivalent(std::string_view(path1), std::string_view(path2));
}

bool Ffs::equivalent(std::string_view path1, std::string_view path2)
{
  // non-existant paths are not equivalent
  std::error_code ec;
  bool b = fs::equivalent(Ffs::expanduser(path1), Ffs::expanduser(path2), ec);
  if (ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:equivalent: " << ec.message() << "\n";
    return false;
  }

  return b;
}


bool fs_copy_file(const char* source, const char* dest, bool overwrite)
{
  try{
    return Ffs::copy_file(std::string_view(source), std::string_view(dest), overwrite);
  } catch(fs::filesystem_error& e){
    std::cerr << "ERROR:ffilesystem:copy_file: " << e.what() << "\n";
    return false;
  }
}

bool Ffs::copy_file(std::string_view source, std::string_view dest, bool overwrite)
{

  if(dest.empty()) UNLIKELY
  {
    std::cerr <<"Ffs::copy_file: destination path must not be empty: " << source << " => " << dest << "\n";
    return false;
  }

  fs::path s(Ffs::canonical(source, true));
  fs::path d(Ffs::canonical(dest, false));

  // auto opt = fs::copy_options::none;
  // if (overwrite)
  //   opt = fs::copy_options::overwrite_existing;
// WORKAROUND: Windows MinGW GCC 11..13, Intel oneAPI Linux: bug with overwrite_existing failing on overwrite

  if(overwrite && fs::is_regular_file(d) && !fs::remove(d)) UNLIKELY
    std::cerr << "ERROR:Ffs::copy_file: could not remove existing destination file: " << d << "\n";

  return fs::copy_file(s, d);
}


size_t fs_relative_to(const char* to, const char* from, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::relative_to(to, from), result, buffer_size);
}

std::string Ffs::relative_to(std::string_view to, std::string_view from)
{
  // fs::relative resolves symlinks and normalizes both paths first

  // undefined case, avoid bugs with MacOS
  if (to.empty() || from.empty()) UNLIKELY
    return {};

  fs::path tp(to);
  fs::path fp(from);
  // cannot be relative, avoid bugs with MacOS
  if(tp.is_absolute() != fp.is_absolute())
    return {};

  std::error_code ec;
  auto r = fs::relative(tp, fp, ec);
  if(ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:relative_to: " << ec.message() << "\n";
    return {};
  }
  return r.generic_string();
}


size_t fs_getenv(const char* name, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::get_env(std::string_view(name)), result, buffer_size);
}

std::string Ffs::get_env(std::string_view name)
{
  if(auto r = std::getenv(name.data()); r && std::strlen(r) > 0)
    return r;

  return {};
}

bool fs_setenv(const char* name, const char* value)
{
  return Ffs::set_env(std::string_view(name), std::string_view(value));
}


bool Ffs::set_env(std::string_view name, std::string_view value)
{
  if(name.empty()) UNLIKELY
  {
    std::cerr << "WARNING:ffilesystem:setenv: name must be non-empty\n";
    return false;
  }

#ifdef _WIN32
  if(std::string v = std::string(name) + "=" + std::string(value); putenv(v.data()))
#else
  if(setenv(name.data(), value.data(), 1))
#endif
  {
    std::cerr << "Ffs:set_env: could not set environment variable " << name << "\n";
    return false;
  }

  return true;
}


size_t fs_which(const char* name, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::which(std::string_view(name)), result, buffer_size);
}

std::string Ffs::which(std::string_view name)
// find full path to executable name on Path
{
  if (name.empty()) UNLIKELY
    return {};

  std::string n(name);

  if (Ffs::is_absolute(n))
    return Ffs::is_exe(n) ? Ffs::normal(n) : std::string();

  const char pathsep = fs_pathsep();

  std::string path = Ffs::get_env("PATH");
  if (path.empty()) UNLIKELY
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
      return Ffs::normal(p);

    start = end + 1;
    end = path.find_first_of(pathsep, start);
  }

  p = path.substr(start) + "/" + n;
  if(Ffs::is_exe(p))
    return Ffs::normal(p);

  return {};
}


bool fs_touch(const char* path)
{
  try{
    Ffs::touch(std::string_view(path));
    return true;
  } catch(fs::filesystem_error& e){
    std::cerr << "ERROR:ffilesystem:touch: " << path << " " << e.what() << "\n";
    return false;
  }
}

void Ffs::touch(std::string_view path)
{
  fs::path p(path);

  auto s = fs::status(p);

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  fs::perms owner_read = fs::perms::owner_read;
  fs::perms owner_write = fs::perms::owner_write;
#endif

  if(fs::exists(s)) {
    fs::last_write_time(p, fs::file_time_type::clock::now());
    return;
  }

  std::ofstream ost;
  ost.open(p, std::ios_base::out | std::ios_base::binary);
  if(!ost.is_open()) UNLIKELY
    throw fs::filesystem_error("filesystem:touch: could not create", p, std::make_error_code(std::errc::no_such_file_or_directory));
  ost.close();

  // ensure user can access file, as default permissions may be mode 600 or such
  fs::permissions(p, owner_read | owner_write, fs::perm_options::add);
}


size_t fs_get_tempdir(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::get_tempdir(), path, buffer_size);

}

std::string Ffs::get_tempdir()
{
  std::error_code ec;
  auto p = fs::temp_directory_path(ec);
  if(ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:get_tempdir: " << ec.message() << "\n";
    return {};
  }
  return p.generic_string();
}


uintmax_t fs_file_size(const char* path)
{
  return Ffs::file_size(std::string_view(path));
}

uintmax_t Ffs::file_size(std::string_view path)
{
  std::error_code ec;
  auto s = fs::file_size(path, ec);
  if(ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:file_size: " << ec.message() << "\n";
    return 0;
  }
  return s;
}


uintmax_t fs_space_available(const char* path)
{
  return Ffs::space_available(std::string_view(path));
}

uintmax_t Ffs::space_available(std::string_view path)
{
  // filesystem space available for device holding path

  std::error_code ec;
  auto s = fs::space(path, ec);
  if(ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:space_available: " << ec.message() << "\n";
    return 0;
  }
  return s.available;
}


size_t fs_get_cwd(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::get_cwd(), path, buffer_size);
}

std::string Ffs::get_cwd()
{
  std::error_code ec;
  auto s = fs::current_path(ec);
  if (ec) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:get_cwd: " << ec.message() << "\n";
    return {};
  }
  return s.generic_string();
}


bool fs_set_cwd(const char *path)
{
  try{
    fs::current_path(path);
    return true;
  } catch (std::filesystem::filesystem_error& e) {
    std::cerr << "ERROR:ffilesystem:set_cwd: " << e.what() << "\n";
    return false;
  }

}

void Ffs::chdir(std::string_view path)
{
  fs::current_path(path);
}


size_t fs_get_homedir(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::get_homedir(), path, buffer_size);
}

std::string Ffs::get_homedir()
{
  // homedir is normalized by definition
  // must be std::string to avoid dangling pointer -- GCC doesn't detect this but Clang does.
  std::string homedir = Ffs::get_env(fs_is_windows() ? "USERPROFILE" : "HOME");
  if(!homedir.empty()) LIKELY
    return Ffs::normal(homedir);

#ifdef _WIN32
  // works on MSYS2, MSVC, oneAPI.
  auto L = static_cast<DWORD>(fs_get_max_path());
  auto buf = std::make_unique<char[]>(L);
  // process with query permission
  HANDLE hToken = nullptr;
  if(!OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken)) UNLIKELY
  {
		CloseHandle(hToken);
    return {};
  }

  bool ok = GetUserProfileDirectoryA(hToken, buf.get(), &L);
  CloseHandle(hToken);
  if (!ok) UNLIKELY
    return {};

  homedir = std::string_view(buf.get());
#else
  const char *h = getpwuid(geteuid())->pw_dir;
  if (!h) UNLIKELY
    return {};
  homedir = std::string_view(h);
#endif

  return Ffs::normal(homedir);
}

size_t fs_expanduser(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::expanduser(std::string_view(path)), result, buffer_size);
}

std::string Ffs::expanduser(std::string_view path)
{
  // The path is also normalized by defintion

  if(path.empty()) UNLIKELY
    return {};
  // cannot call .front() on empty string_view() (MSVC)

  if(path.front() != '~')
    return Ffs::normal(path);

  std::set <char> filesep = {'/', fs::path::preferred_separator};

  if(path.length() > 1 &&
     // second character is not a file separator
#if __cplusplus >= 202002L
  !filesep.contains(path[1])
#else
  filesep.find(path[1]) == filesep.end()
#endif
  )
    return Ffs::normal(path);


  std::string h = Ffs::get_homedir();
  if (h.empty()) UNLIKELY
  {
    std::cerr << "ERROR:ffilesystem:expanduser: could not get home directory\n";
    return {};
  }
  if (path.length() < 3)
    return h;

// handle initial duplicated file separators. NOT .lexical_normal to handle "~/.."
  size_t i = 2;
  while(i < path.length() &&
#if __cplusplus >= 202002L
  filesep.contains(path[i])
#else
  filesep.find(path[i]) != filesep.end()
#endif
  )
    i++;

  // The path is also normalized by definition
  return Ffs::normal((fs::path(h) / path.substr(i)).generic_string());
}


bool fs_is_subdir(const char* subdir, const char* dir)
{
  return Ffs::is_subdir(std::string_view(subdir), std::string_view(dir));
}

bool Ffs::is_subdir(std::string_view subdir, std::string_view dir)
{
  // subdir is a subdirectory of dir -- lexical operation

  std::string s = Ffs::normal(subdir);
  std::string d = Ffs::normal(dir);

  return (s.length() > d.length()) && s.compare(0, d.length(), d) == 0;
}


bool fs_set_permissions(const char* path, int readable, int writable, int executable)
{
  // make path file owner readable or not
  try{
    Ffs::set_permissions(std::string_view(path), readable, writable, executable);
    return true;
  } catch(fs::filesystem_error& e){
    std::cerr << "ERROR:Ffilesystem:set_permissions: " << readable << " " << e.what() << "\n";
    return false;
  }
}

void Ffs::set_permissions(std::string_view path, int readable, int writable, int executable)
{

  fs::path pth(path);

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  fs::perms owner_read = fs::perms::owner_read;
  fs::perms owner_write = fs::perms::owner_write;
  fs::perms owner_exec = fs::perms::owner_exec;
#endif

  if (readable != 0)
    fs::permissions(pth, owner_read,
      (readable > 0) ? fs::perm_options::add : fs::perm_options::remove);

  if (writable != 0)
    fs::permissions(pth, owner_write,
      (writable > 0) ? fs::perm_options::add : fs::perm_options::remove);

  if (executable != 0)
    fs::permissions(pth, owner_exec,
      (executable > 0) ? fs::perm_options::add : fs::perm_options::remove);

}


size_t fs_exe_path(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::exe_path(), path, buffer_size);
}

std::string Ffs::exe_path()
{
  // https://stackoverflow.com/a/4031835
  // https://stackoverflow.com/a/1024937

  auto buf = std::make_unique<char[]>(fs_get_max_path());

#ifdef _WIN32
 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  if (!GetModuleFileNameA(nullptr, buf.get(), static_cast<DWORD>(fs_get_max_path()))) UNLIKELY
    return {};
#elif defined(__linux__) || defined(__CYGWIN__)
  // https://man7.org/linux/man-pages/man2/readlink.2.html
  size_t L = readlink("/proc/self/exe", buf.get(), fs_get_max_path());
  if (L < 1 || L >= fs_get_max_path()) UNLIKELY
    return {};
#elif defined(__APPLE__)
  uint32_t mp = fs_get_max_path();
  if(_NSGetExecutablePath(buf.get(), &mp)) UNLIKELY
    return {};
#else
  std::cerr << "ERROR:ffilesystem:exe_path: not implemented for this platform\n";
  return {};
#endif

  std::string s(buf.get());
  return Ffs::canonical(s, true);
}


size_t fs_get_permissions(const char* path, char* result, size_t buffer_size)
{
  return fs_str2char(Ffs::get_permissions(std::string_view(path)), result, buffer_size);
}

std::string Ffs::get_permissions(std::string_view path)
{

  std::error_code ec;
  auto s = fs::status(path, ec);
  if(ec || !fs::exists(s)) UNLIKELY
    return {};

  fs::perms p = s.permissions();

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  fs::perms none = fs::perms::none;
  fs::perms owner_read = fs::perms::owner_read;
  fs::perms owner_write = fs::perms::owner_write;
  fs::perms owner_exec = fs::perms::owner_exec;
  fs::perms group_read = fs::perms::group_read;
  fs::perms group_write = fs::perms::group_write;
  fs::perms group_exec = fs::perms::group_exec;
  fs::perms others_read = fs::perms::others_read;
  fs::perms others_write = fs::perms::others_write;
  fs::perms others_exec = fs::perms::others_exec;
#endif

  std::string r = "---------";
  if ((p & owner_read) != none)
    r[0] = 'r';
  if ((p & owner_write) != none)
    r[1] = 'w';
  if ((p & owner_exec) != none)
    r[2] = 'x';
  if ((p & group_read) != none)
    r[3] = 'r';
  if ((p & group_write) != none)
    r[4] = 'w';
  if ((p & group_exec) != none)
    r[5] = 'x';
  if ((p & others_read) != none)
    r[6] = 'r';
  if ((p & others_write) != none)
    r[7] = 'w';
  if ((p & others_exec) != none)
    r[8] = 'x';

  return r;
}


size_t fs_lib_path(char* path, size_t buffer_size)
{
  return fs_str2char(Ffs::lib_path(), path, buffer_size);
}

std::string Ffs::lib_path()
{
#if (defined(_WIN32) || defined(__CYGWIN__)) && defined(FS_DLL_NAME)
  auto buf = std::make_unique<char[]>(fs_get_max_path());

 // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  if(!GetModuleFileNameA(GetModuleHandleA(FS_DLL_NAME), buf.get(), fs_get_max_path())) UNLIKELY
    return {};

  std::string s(buf.get());
  return s;
#elif defined(HAVE_DLADDR)
  Dl_info info;

  return dladdr( (void*)&dl_dummy_func, &info)
    ? std::string(info.dli_fname)
    : std::string();
#else
  return {};
#endif
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

  std::string buf = Ffs::expanduser(base);

  return Ffs::join(buf, out);
}

// --- mkdtemp

size_t fs_make_tempdir(char* result, size_t buffer_size){
  try{
    return fs_str2char(Ffs::mkdtemp("tmp."), result, buffer_size);
  } catch(fs::filesystem_error& e) {
    std::cerr << "ERROR:ffilesystem:make_tempdir: " << e.what() << "\n";
    return 0;
  }
}


std::string Ffs::mkdtemp(std::string_view prefix)
{
  // make unique temporary directory starting with prefix

  fs::path t;
  size_t Lname = 16;  // arbitrary length for random string
  fs::path temp = fs::temp_directory_path();

  do {
    t = (temp / (prefix.data() + fs_generate_random_alphanumeric_string(Lname)));
    if(FS_TRACE) std::cout << "TRACE:make_tempdir: " << t << "\n";
  } while (fs::is_directory(t));

  if (!fs::create_directory(t)) UNLIKELY
  {
    std::cerr << "Ffs::mkdtemp:mkdir: could not create temporary directory " << t << "\n";
    return {};
  }

  return t.generic_string();
}

// CTAD C++17 random string generator
// https://stackoverflow.com/a/444614

template <typename T = std::mt19937>
static auto fs_random_generator() -> T {
    auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
    auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
    auto seed = std::array<std::seed_seq::result_type, seed_len>();
    auto dev = std::random_device();
    std::generate_n(begin(seed), seed_len, std::ref(dev));
    auto seed_seq = std::seed_seq(begin(seed), end(seed));
    return T{seed_seq};
}

static std::string fs_generate_random_alphanumeric_string(std::size_t len)
{
    static constexpr auto chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    thread_local auto rng = fs_random_generator<>();
    auto dist = std::uniform_int_distribution{{}, std::strlen(chars) - 1};
    auto result = std::string(len, '\0');
    std::generate_n(begin(result), len, [&]() { return chars[dist(rng)]; });
    return result;
}
// --- end mkdtemp

size_t fs_shortname(const char* in, char* out, size_t buffer_size){
  return fs_str2char(Ffs::shortname(std::string_view(in)), out, buffer_size);
}

std::string Ffs::shortname(std::string_view in){
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getshortpathnamew
// the path must exist

  fs::path p(in);

#ifdef _WIN32
  auto buf = std::make_unique<char[]>(fs_get_max_path());
// size includes null terminator
  DWORD L = GetShortPathNameA(in.data(), nullptr, 0);
  if (L == 0) UNLIKELY
  {
    std::cerr << "ERROR:Ffs::shortname:GetShortPathName: could not determine short path length " << p << "\n";
    return {};
  }

// convert long path
  if(!GetShortPathNameA(in.data(), buf.get(), L)) UNLIKELY
  {
    std::cerr << "ERROR:Ffs::shortname:GetShortPathName: could not determine short path " << p << "\n";
    return {};
  }

  std::string out(buf.get());
#else
  std::cerr << "WARNING:Ffs::shortname:ffilesystem: Windows-only\n";
  std::string out(in);
#endif
  return out;
}


size_t fs_longname(const char* in, char* out, size_t buffer_size){
  return fs_str2char(Ffs::longname(std::string_view(in)), out, buffer_size);
}

std::string Ffs::longname(std::string_view in){
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea
// the path must exist

  fs::path p(in);

#ifdef _WIN32
    auto buf = std::make_unique<char[]>(fs_get_max_path());
// size includes null terminator
    DWORD L = GetLongPathNameA(in.data(), nullptr, 0);
    if(L == 0) UNLIKELY
    {
    std::cerr << "ERROR:Ffs::longname:GetLongPathName: could not determine path length " << p << "\n";
    return {};
  }

// convert short path
    if(!GetLongPathNameA(in.data(), buf.get(), L)) UNLIKELY
    {
    std::cerr << "ERROR:Ffs::longname:GetLongPathName: could not determine path " << p << "\n";
    return {};
  }
    std::string out(buf.get());
#else
    std::cerr << "WARNING:ffilesystem:Ffs::longname: Windows-only\n";
    std::string out(in);
#endif

    return out;
}

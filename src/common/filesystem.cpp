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

#include <iostream>
#include <algorithm>
#include <array>
#include <functional>
#include <random>
#include <iterator> // std::begin, std::end
#include <cstring>  // std::strlen
#include <string>
#include <fstream>
#include <set>
#include <cstddef> // size_t
#include <cstdint>
#include <cstdlib>
#include <system_error>
#include <cctype> // std::isalnum

#include <sys/types.h>

#if __has_include(<ranges>)
#include <ranges>
#endif

#include <ctime> // std::time_t

#include <filesystem>

#include "ffilesystem.h"


static std::string fs_generate_random_alphanumeric_string(std::size_t);


bool fs_mkdir(const char* path)
{
  return Ffs::mkdir(std::string_view(path));
}

bool Ffs::mkdir(std::string_view path)
{

  if (Ffs::is_dir(path))
    return true;

  std::error_code ec;
  // old MacOS return false even if directory was created
  if (std::filesystem::create_directories(path, ec) || (!ec && Ffs::is_dir(path))) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:mkdir: " << ec.message() << "\n";
  return false;
}


bool fs_remove(const char* path)
{
  return Ffs::remove(std::string_view(path));
}

bool Ffs::remove(std::string_view path)
{
  std::error_code ec;

  if(std::filesystem::remove(path, ec) && !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:remove: " << ec.message() << "\n";
  return false;
}


bool fs_copy_file(const char* source, const char* dest, bool overwrite)
{
  return Ffs::copy_file(std::string_view(source), std::string_view(dest), overwrite);
}

bool Ffs::copy_file(std::string_view source, std::string_view dest, bool overwrite)
{
  auto opt = std::filesystem::copy_options::none;
  if (overwrite)
    opt = std::filesystem::copy_options::overwrite_existing;
// WORKAROUND: Windows MinGW GCC 11..13, Intel oneAPI Linux: bug with overwrite_existing failing on overwrite

  if(overwrite && Ffs::is_file(dest) && !Ffs::remove(dest)) FFS_UNLIKELY
    std::cerr << "ERROR:Ffs::copy_file: could not remove existing destination file: " << dest << "\n";

  std::error_code ec;
  if(std::filesystem::copy_file(source, dest, opt, ec) && !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:copy_file: " << ec.message() << "\n";
  return false;
}


bool fs_set_modtime(const char* path)
{
  return Ffs::set_modtime(std::string_view(path));
}

bool Ffs::set_modtime(std::string_view path)
{
  std::error_code ec;

  std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now(), ec);
  if(!ec) FFS_LIKELY
    return true;
  // techinically IWYU <chrono> but that can break some compilers, and it works without the include.

  std::cerr << "ERROR:ffilesystem:set_modtime: " << ec.message() << "\n";
  return false;
}


bool fs_touch(const char* path)
{
  try{
    Ffs::touch(std::string_view(path));
    return true;
  } catch(std::filesystem::filesystem_error& e){
    std::cerr << "ERROR:ffilesystem:touch: " << path << " " << e.what() << "\n";
    return false;
  }
}

void Ffs::touch(std::string_view path)
{
  std::filesystem::path p(path);

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
#endif

  if(std::filesystem::exists(p)) {
    std::filesystem::last_write_time(p, std::filesystem::file_time_type::clock::now());
    // techinically IWYU <chrono> but that can break some compilers, and it works without the include.
    return;
  }

  std::ofstream ost;
  ost.open(p, std::ios_base::out | std::ios_base::binary);
  if(!ost.is_open()) FFS_UNLIKELY
    throw std::filesystem::filesystem_error("filesystem:touch: could not create", p, std::make_error_code(std::errc::no_such_file_or_directory));
  ost.close();

  // ensure user can access file, as default permissions may be mode 600 or such
  std::filesystem::permissions(p, owner_read | owner_write, std::filesystem::perm_options::add);
}


bool fs_set_permissions(const char* path, int readable, int writable, int executable)
{
  // make path file owner readable or not
  try{
    Ffs::set_permissions(std::string_view(path), readable, writable, executable);
    return true;
  } catch(std::filesystem::filesystem_error& e){
    std::cerr << "ERROR:Ffilesystem:set_permissions: " << readable << " " << e.what() << "\n";
    return false;
  }
}

void Ffs::set_permissions(std::string_view path, int readable, int writable, int executable)
{

  std::filesystem::path pth(path);

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
  std::filesystem::perms owner_exec = std::filesystem::perms::owner_exec;
#endif

  if (readable != 0)
    std::filesystem::permissions(pth, owner_read,
      (readable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove);

  if (writable != 0)
    std::filesystem::permissions(pth, owner_write,
      (writable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove);

  if (executable != 0)
    std::filesystem::permissions(pth, owner_exec,
      (executable > 0) ? std::filesystem::perm_options::add : std::filesystem::perm_options::remove);

}


// --- mkdtemp

size_t fs_make_tempdir(char* result, size_t buffer_size){
  return fs_str2char(Ffs::mkdtemp("tmp."), result, buffer_size);
}

std::string Ffs::mkdtemp(std::string_view prefix)
{
  // make unique temporary directory starting with prefix

  std::error_code ec;
  std::filesystem::path t;
  size_t Lname = 16;  // arbitrary length for random string
  std::filesystem::path temp = std::filesystem::temp_directory_path(ec);

  if(!ec) FFS_LIKELY
  {
    do {
      t = (temp / (prefix.data() + fs_generate_random_alphanumeric_string(Lname)));
      if(FS_TRACE) std::cout << "TRACE:make_tempdir: " << t << "\n";
    } while (std::filesystem::is_directory(t, ec) && !ec);

    if (std::filesystem::create_directory(t, ec) && !ec) FFS_LIKELY
      return t.generic_string();
  }

  std::cerr << "Ffs::mkdtemp:mkdir: could not create temporary directory " << ec.message() << "\n";
  return {};
}

// CTAD C++17 random string generator
// https://stackoverflow.com/a/444614

template <typename T = std::mt19937>
static auto fs_random_generator() -> T {
    auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
    auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
    auto seed = std::array<std::seed_seq::result_type, seed_len>();
    auto dev = std::random_device();
    std::generate_n(std::begin(seed), seed_len, std::ref(dev));
    auto seed_seq = std::seed_seq(std::begin(seed), std::end(seed));
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
    std::generate_n(std::begin(result), len, [&]() { return chars[dist(rng)]; });
    return result;
}
// --- end mkdtemp

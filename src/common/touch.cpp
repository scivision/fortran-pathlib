#include "ffilesystem.h"

#include <system_error>
#include <iostream>
#include <string>

#include <fstream> // std::ofstream



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


bool Ffs::touch(std::string_view path)
{

#if defined(__cpp_using_enum)
  using enum std::filesystem::perms;
#else
  constexpr std::filesystem::perms owner_read = std::filesystem::perms::owner_read;
  constexpr std::filesystem::perms owner_write = std::filesystem::perms::owner_write;
#endif

  std::error_code ec;

  if(Ffs::exists(path)) {
    std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now(), ec);
    // techinically IWYU <chrono> but that can break some compilers, and it works without the include.
    return !ec;
  }

  std::ofstream ost;
  ost.open(path.data(), std::ios_base::out | std::ios_base::binary);
  if(!ost.is_open()){ FFS_UNLIKELY
    std::cerr << "ERROR:ffilesystem:touch: could not open file " << path << "\n";
    return false;
  }

  ost.close();

  // ensure user can access new file, as default permissions may be mode 600 or such
  std::filesystem::permissions(path, owner_read | owner_write, std::filesystem::perm_options::add, ec);
  if(!ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:touch: " << ec.message() << "\n";
  return false;
}

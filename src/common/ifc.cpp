#include "ffilesystem.h"

#include <string>


std::string Ffs::filesystem_type(std::string_view path)
{
  if(std::string name(80, '\0');
      fs_filesystem_type(path.data(), name.data(), name.size()))
    return name.c_str();

  return {};
}

std::string Ffs::get_hostname(){ return fs_hostname(); }


std::string Ffs::realpath(std::string_view path) { return fs_realpath(path); }


std::string Ffs::get_username(){ return fs_get_username(); }


bool Ffs::set_env(std::string_view name, std::string_view value)
{
  return fs_setenv(name, value);
}


std::string Ffs::get_env(std::string_view name) { return fs_getenv(name); }


bool Ffs::is_reserved(std::string_view filename)
{
  return fs_is_reserved(filename.data());
}


bool Ffs::is_safe_name(std::string_view filename)
{
  return fs_is_safe_name(filename.data());
}


bool Ffs::is_subdir(std::string_view subdir, std::string_view dir)
{
  return fs_is_subdir(subdir.data(), dir.data());
}


bool Ffs::touch(std::string_view path)
{
  return fs_touch(path);
}

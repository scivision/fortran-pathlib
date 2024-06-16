#include "ffilesystem.h"

#include <string>


std::string Ffs::as_posix(std::string_view path)
{
  std::string buf(path);

  fs_as_posix(buf.data());
  return buf;
}


std::string Ffs::compiler()
{
  if(std::string buf(80, '\0');
      fs_compiler(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::cpu_arch()
{
  if(std::string buf(80, '\0');
      fs_cpu_arch(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::exe_path()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_exe_path(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::expanduser(std::string_view path)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_expanduser(path.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::filesystem_type(std::string_view path)
{
  if(std::string name(80, '\0');
      fs_filesystem_type(path.data(), name.data(), name.size()))
    return name.c_str();

  return {};
}


std::string Ffs::get_homedir()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_get_homedir(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


bool Ffs::is_safe_name(std::string_view filename)
{
  return fs_is_safe_name(filename.data());
}


bool Ffs::is_subdir(std::string_view subdir, std::string_view dir)
{
  return fs_is_subdir(subdir.data(), dir.data());
}


std::string Ffs::lib_path()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_lib_path(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}

std::string Ffs::shortname(std::string_view in)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_shortname(in.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::longname(std::string_view in)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_longname(in.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::to_cygpath(std::string_view in)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_to_cygpath(in.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::to_winpath(std::string_view in)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_to_winpath(in.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}

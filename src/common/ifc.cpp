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
  // 1000 is arbitrary
  if(std::string buf(1000, '\0');
      fs_compiler(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::get_shell()
{
  if(std::string buf(80, '\0');
      fs_get_shell(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::get_terminal()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_get_terminal(buf.data(), buf.size()) > 0)  FFS_LIKELY
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


size_t Ffs::max_component(std::string_view path)
{
  return fs_max_component(path.data());
}


std::string Ffs::get_homedir()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_get_homedir(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::get_profile_dir()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_get_profile_dir(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::user_config_dir()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_user_config_dir(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::get_owner_name(std::string_view path)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_get_owner_name(path.data(), buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::get_owner_group(std::string_view path)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_get_owner_group(path.data(), buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::get_hostname(){ return fs_hostname(); }


std::string Ffs::realpath(std::string_view path)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_realpath(path.data(), buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}


std::string Ffs::get_username()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_get_username(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};

}


bool Ffs::set_env(std::string_view name, std::string_view value)
{
  return fs_setenv(name.data(), value.data());
}


std::string Ffs::get_env(std::string_view name)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_getenv(name.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}


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


std::string Ffs::lib_path()
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_lib_path(buf.data(), buf.size()) > 0)  FFS_LIKELY
    return buf.c_str();

  return {};
}

std::string Ffs::shortname(std::string_view in){ return fs_shortname(in); }
std::string Ffs::longname(std::string_view in){ return fs_longname(in); }


bool Ffs::touch(std::string_view path)
{
  return fs_touch(path.data());
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


std::string Ffs::which(std::string_view name)
{
  if(std::string buf(fs_get_max_path(), '\0');
      fs_which(name.data(), buf.data(), buf.size())) FFS_LIKELY
    return buf.c_str();

  return {};
}

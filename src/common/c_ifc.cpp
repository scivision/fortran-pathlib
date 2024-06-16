#include "ffilesystem.h"


bool fs_exists(const char* path){ return Ffs::exists(std::string_view(path)); }

bool fs_is_absolute(const char* path){ return Ffs::is_absolute(std::string_view(path)); }

bool fs_is_char_device(const char* path){ return Ffs::is_char_device(std::string_view(path)); }

bool fs_is_dir(const char* path){ return Ffs::is_dir(std::string_view(path)); }

bool fs_is_exe(const char* path){ return Ffs::is_exe(std::string_view(path)); }

bool fs_is_file(const char* path){ return Ffs::is_file(std::string_view(path)); }

bool fs_is_reserved(const char* path){ return Ffs::is_reserved(std::string_view(path)); }

bool fs_is_readable(const char* path){ return Ffs::is_readable(std::string_view(path)); }

bool fs_is_symlink(const char* path){ return Ffs::is_symlink(std::string_view(path)); }

bool fs_is_writable(const char* path){ return Ffs::is_writable(std::string_view(path)); }

uintmax_t fs_file_size(const char* path){ return Ffs::file_size(std::string_view(path)); }

uintmax_t fs_space_available(const char* path){ return Ffs::space_available(std::string_view(path)); }


std::string::size_type fs_canonical(const char* path, const bool strict,
                          char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::canonical(std::string_view(path), strict), result, buffer_size);
}


bool fs_create_symlink(const char* target, const char* link){
  return Ffs::create_symlink(std::string_view(target), std::string_view(link));
}


bool fs_equivalent(const char* path1, const char* path2){
  return Ffs::equivalent(std::string_view(path1), std::string_view(path2));
}


std::string::size_type fs_get_cwd(char* path, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::get_cwd(), path, buffer_size);
}

std::string::size_type fs_getenv(const char* name,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::get_env(std::string_view(name)), result, buffer_size);
}

std::string::size_type fs_get_permissions(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::get_permissions(std::string_view(path)), result, buffer_size);
}


std::string::size_type fs_expanduser(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::expanduser(std::string_view(path)), result, buffer_size);
}

std::string::size_type fs_file_name(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::file_name(std::string_view(path)), result, buffer_size);
}

std::string::size_type fs_join(const char* path, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::join(std::string_view(path), std::string_view(other)), result, buffer_size);
}

std::string::size_type fs_make_absolute(const char* path, const char* base,
                          char* out, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::make_absolute(std::string_view(path), std::string_view(base)), out, buffer_size);
}

std::string::size_type fs_normal(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::normal(std::string_view(path)), result, buffer_size);
}

std::string::size_type fs_parent(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::parent(std::string_view(path)), result, buffer_size);
}

std::string::size_type fs_proximate_to(const char* base, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::proximate_to(base, other), result, buffer_size);
}

std::string::size_type fs_read_symlink(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::read_symlink(std::string_view(path)), result, buffer_size);
}

std::string::size_type fs_relative_to(const char* base, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::relative_to(base, other), result, buffer_size);
}

std::string::size_type fs_resolve(const char* path, const bool strict,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::resolve(std::string_view(path), strict), result, buffer_size);
}

std::string::size_type fs_root(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::root(std::string_view(path)), result, buffer_size);
}

std::string::size_type fs_stem(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::stem(std::string_view(path)), result, buffer_size);
}

std::string::size_type fs_suffix(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::suffix(std::string_view(path)), result, buffer_size);
}

std::string::size_type fs_which(const char* name,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::which(std::string_view(name)), result, buffer_size);
}

std::string::size_type fs_with_suffix(const char* path, const char* new_suffix,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::with_suffix(std::string_view(path), std::string_view(new_suffix)), result, buffer_size);
}

bool fs_setenv(const char* name, const char* value){ return Ffs::set_env(std::string_view(name), std::string_view(value)); }

bool fs_set_cwd(const char *path){ return Ffs::chdir(path); }

std::string::size_type fs_get_tempdir(char* path, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::get_tempdir(), path, buffer_size);
}


bool fs_copy_file(const char* source, const char* dest, bool overwrite){
  return Ffs::copy_file(std::string_view(source), std::string_view(dest), overwrite);
}

bool fs_mkdir(const char* path){ return Ffs::mkdir(std::string_view(path)); }

bool fs_remove(const char* path){ return Ffs::remove(std::string_view(path)); }

bool fs_set_modtime(const char* path){ return Ffs::set_modtime(std::string_view(path)); }

std::string::size_type fs_make_tempdir(char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::mkdtemp("tmp."), result, buffer_size);
}

bool fs_set_permissions(const char* path, int readable, int writable, int executable){
    return Ffs::set_permissions(std::string_view(path), readable, writable, executable);
}

bool fs_touch(const char* path){ return Ffs::touch(std::string_view(path)); }

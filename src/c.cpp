// interfaces for C

#include <cstdint> // size_t
#include <iostream>
#include <string>

#include "ffilesystem.h"

std::string::size_type fs_str2char(std::string_view s, char* result, const std::string::size_type buffer_size)
{
  if(s.length() >= buffer_size) FFS_UNLIKELY
  {
    result = nullptr;
    std::cerr << "ERROR:Ffs:str2char(" << s << ") output buffer " << buffer_size << " too small for string: " << s << " length " << s.length() << "\n";
    return 0;
  }

  s.copy(result, buffer_size);
  result[s.length()] = '\0';
  return s.length();
}

std::string::size_type fs_absolute(const char* path, const char* base, const bool expand_tilde,
                          char* out, const std::string::size_type buffer_size){
  return fs_str2char(fs_absolute(path, base, expand_tilde), out, buffer_size);
}

std::string::size_type fs_parent(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_parent(path), result, buffer_size);
}


std::string::size_type fs_suffix(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_suffix(path), result, buffer_size);
}

std::string::size_type fs_mkdtemp(const char* tmpl, char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_mkdtemp(tmpl), result, buffer_size);
}

std::string::size_type fs_join(const char* path, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_join(path, other), result, buffer_size);
}

std::string::size_type fs_normal(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_normal(path), result, buffer_size);
}

std::string::size_type fs_canonical(const char* path, const bool strict, const bool expand_tilde,
                          char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_canonical(path, strict, expand_tilde).value_or(""), result, buffer_size);
}

std::string::size_type fs_resolve(const char* path, const bool strict, const bool expand_tilde,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_resolve(path, strict, expand_tilde).value_or(""), result, buffer_size);
}


bool fs_equivalent(const char* path1, const char* path2){
  return fs_equivalent(std::string_view(path1), std::string_view(path2));
}

std::string::size_type fs_relative_to(const char* base, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_relative_to(base, other), result, buffer_size);
}

std::string::size_type fs_proximate_to(const char* base, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_proximate_to(base, other), result, buffer_size);
}

bool fs_is_subdir(const char* subdir, const char* dir)
{
  return fs_is_subdir(std::string_view(subdir), std::string_view(dir));
}


bool fs_mkdir(const char* path){ return fs_mkdir(std::string_view(path)); }

size_t fs_longname(const char* in, char* result, const size_t buffer_size){
  return fs_str2char(fs_longname(in), result, buffer_size);
}


size_t fs_shortname(const char* in, char* result, const size_t buffer_size){
  return fs_str2char(fs_shortname(in), result, buffer_size);
}


void fs_print_error(const char* msg, const char* path){
  fs_print_error(std::string_view(msg), std::string_view(path));
}

bool fs_win32_create_symlink(const char* target, const char* link){
  return fs_win32_create_symlink(std::string_view(target), std::string_view(link));
}

bool fs_copy_file(const char* source, const char* dest, bool overwrite){
  return fs_copy_file(std::string_view(source), std::string_view(dest), overwrite);
}

size_t fs_hostname(char* name, const size_t buffer_size)
{
  return fs_str2char(fs_hostname(), name, buffer_size);
}

size_t fs_cpu_arch(char* arch, const size_t buffer_size)
{
  return fs_str2char(fs_cpu_arch(), arch, buffer_size);
}

size_t fs_get_terminal(char* name, const size_t buffer_size)
{
  return fs_str2char(fs_get_terminal(), name, buffer_size);
}

size_t fs_get_shell(char* name, const size_t buffer_size)
{
  return fs_str2char(fs_get_shell(), name, buffer_size);
}


size_t fs_getenv(const char* name, char* value, const size_t buffer_size)
{
  return fs_str2char(fs_getenv(name), value, buffer_size);
}

bool fs_setenv(const char* name, const char* value)
{
  return fs_setenv(std::string_view(name), std::string_view(value));
}

size_t fs_get_homedir(char* path, const size_t buffer_size)
{
  return fs_str2char(fs_get_homedir(), path, buffer_size);
}

size_t fs_get_profile_dir(char* path, const size_t buffer_size)
{
  return fs_str2char(fs_get_profile_dir(), path, buffer_size);
}

size_t fs_user_config_dir(char* path, const size_t buffer_size)
{
  return fs_str2char(fs_user_config_dir(), path, buffer_size);
}

size_t fs_get_username(char* name, const size_t buffer_size)
{
  return fs_str2char(fs_get_username(), name, buffer_size);
}

size_t fs_expanduser(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(fs_expanduser(path), result, buffer_size);
}

bool fs_touch(const char* path)
{
  return fs_touch(std::string_view(path));
}

size_t fs_compiler(char* name, const size_t buffer_size)
{
  return fs_str2char(fs_compiler(), name, buffer_size);
}

std::string::size_type fs_max_component(const char* path)
{
  return fs_max_component(std::string_view(path));
}

size_t fs_lib_path(char* path, const size_t buffer_size)
{
  return fs_str2char(fs_lib_path(), path, buffer_size);
}

size_t fs_exe_path(char* path, const size_t buffer_size)
{
  return fs_str2char(fs_exe_path(), path, buffer_size);
}

size_t fs_realpath(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(fs_realpath(path), result, buffer_size);
}

size_t fs_root(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(fs_root(path), result, buffer_size);
}

size_t fs_get_owner_name(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(fs_get_owner_name(path), result, buffer_size);
}

size_t fs_get_owner_group(const char* path, char* result, const size_t buffer_size)
{
  return fs_str2char(fs_get_owner_group(path), result, buffer_size);
}

std::string::size_type fs_stem(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_stem(path), result, buffer_size);
}

std::string::size_type fs_get_cwd(char* path, const std::string::size_type buffer_size){
  return fs_str2char(fs_get_cwd().value_or(""), path, buffer_size);
}

bool fs_set_cwd(const char *path){ return fs_set_cwd(std::string_view(path)); }

std::string::size_type fs_get_tempdir(char* path, const std::string::size_type buffer_size){
  return fs_str2char(fs_get_tempdir().value_or(""), path, buffer_size);
}

bool fs_is_absolute(const char* path)
{
  return fs_is_absolute(std::string_view(path));
}

std::uintmax_t fs_space_available(const char* path){
  return fs_space_available(std::string_view(path));
}

bool fs_set_permissions(const char* path, int readable, int writable, int executable){
  return fs_set_permissions(std::string_view(path), readable, writable, executable);
}

std::string::size_type fs_file_name(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_file_name(path), result, buffer_size);
}

std::string::size_type fs_filesystem_type(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_filesystem_type(path), result, buffer_size);
}

bool fs_set_modtime(const char* path){ return fs_set_modtime(std::string_view(path)); }

bool fs_is_reserved(const char* path){ return fs_is_reserved(std::string_view(path)); }

bool fs_is_safe_name(const char* path){ return fs_is_safe_name(std::string_view(path)); }

bool fs_remove(const char* path){ return fs_remove(std::string_view(path)); }

bool fs_is_char_device(const char* path){ return fs_is_char_device(std::string_view(path)); }

bool fs_is_dir(const char* path){ return fs_is_dir(std::string_view(path)); }

bool fs_is_exe(const char* path){ return fs_is_exe(std::string_view(path)); }

bool fs_exists(const char* path){ return fs_exists(std::string_view(path)); }

bool fs_is_file(const char* path){ return fs_is_file(std::string_view(path)); }

bool fs_is_readable(const char* path){ return fs_is_readable(std::string_view(path)); }

bool fs_is_writable(const char* path){ return fs_is_writable(std::string_view(path)); }

bool fs_create_symlink(const char* target, const char* link){
  return fs_create_symlink(std::string_view(target), std::string_view(link));
}

bool fs_is_symlink(const char* path){ return fs_is_symlink(std::string_view(path)); }

std::string::size_type fs_read_symlink(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_read_symlink(path).value_or(""), result, buffer_size);
}

std::string::size_type fs_get_permissions(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_get_permissions(path).value_or(""), result, buffer_size);
}

uintmax_t fs_file_size(const char* path){
  return fs_file_size(std::string_view(path));
}

size_t fs_to_cygpath(const char* path, char* result, const size_t buffer_size){
  return fs_str2char(fs_to_cygpath(path), result, buffer_size);
}

size_t fs_to_winpath(const char* path, char* result, const size_t buffer_size){
  return fs_str2char(fs_to_winpath(path), result, buffer_size);
}


size_t fs_which(const char* name, char* result, const size_t buffer_size){
  return fs_str2char(fs_which(name), result, buffer_size);
}

std::string::size_type fs_with_suffix(const char* path, const char* new_suffix,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(fs_with_suffix(path, new_suffix), result, buffer_size);
}

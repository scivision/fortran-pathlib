#include <cstdint>
#include <string>

#include "ffilesystem.h"

std::string::size_type fs_canonical(const char* path, const bool strict, const bool expand_tilde,
                          char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::canonical(path, strict, expand_tilde).value_or(""), result, buffer_size);
}


bool fs_equivalent(const char* path1, const char* path2){
  return Ffs::equivalent(path1, path2);
}

std::string::size_type fs_join(const char* path, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::join(path, other), result, buffer_size);
}

std::string::size_type fs_parent(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::parent(path), result, buffer_size);
}

std::string::size_type fs_proximate_to(const char* base, const char* other,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::proximate_to(base, other), result, buffer_size);
}

std::string::size_type fs_resolve(const char* path, const bool strict, const bool expand_tilde,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::resolve(path, strict, expand_tilde).value_or(""), result, buffer_size);
}

std::string::size_type fs_suffix(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::suffix(path), result, buffer_size);
}

std::string::size_type fs_with_suffix(const char* path, const char* new_suffix,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::with_suffix(path, new_suffix), result, buffer_size);
}

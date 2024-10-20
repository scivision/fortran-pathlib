#include <cstdint>
#include <string>

#include "ffilesystem.h"


std::string::size_type fs_parent(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::parent(path), result, buffer_size);
}


std::string::size_type fs_suffix(const char* path,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::suffix(path), result, buffer_size);
}

std::string::size_type fs_with_suffix(const char* path, const char* new_suffix,
                         char* result, const std::string::size_type buffer_size){
  return fs_str2char(Ffs::with_suffix(path, new_suffix), result, buffer_size);
}

// interfaces for C

#include <cstdint> // size_t
#include <iostream>

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

size_t fs_hostname(char* name, const size_t buffer_size)
{
  return fs_str2char(fs_hostname(), name, buffer_size);
}

size_t fs_cpu_arch(char* arch, const size_t buffer_size)
{
  return fs_str2char(fs_cpu_arch(), arch, buffer_size);
}

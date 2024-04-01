#include <cstring>
#include <regex>
#include <filesystem>

#include "myfs.h"

static_assert(__cpp_lib_filesystem, "No C++ filesystem support");


bool has_filename(const char* path){
  std::filesystem::path p(path);
  return p.has_filename();
}

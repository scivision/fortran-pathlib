#include <string_view>
#include <fstream> // std::ofstream

#include "ffilesystem.h"


bool fs_touch(std::string_view path)
{
  if(fs_exists(path))
    return fs_set_modtime(path);

  if(std::ofstream f(path.data()); f){
    f.close();
    return true;
  }

  fs_print_error(path, "touch");
  return false;
}

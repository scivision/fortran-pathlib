#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <string_view>

#include "ffilesystem.h"


bool fs_touch(std::string_view path)
{
  if(fs_exists(path))
    return fs_set_modtime(path.data());

  FILE* fid = fopen(path.data(), "w");
  if(!fid){
    fs_print_error(path, "touch:fopen");
    return false;
  }

  if(fclose(fid) != 0){
    fs_print_error(path, "touch:fclose");
    return false;
  }

  if(fs_is_file(path.data()))
    return true;
  // is_file check because sometimes fclose() doesn't fail, but the file is not created

  fs_print_error(std::string(path) + " was not created", "touch");
  return false;
}

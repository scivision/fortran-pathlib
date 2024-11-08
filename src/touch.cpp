#include <string_view>
#include <cstdio> // fopen, fclose

#include "ffilesystem.h"


bool fs_touch(std::string_view path)
{
  if(fs_exists(path))
    return fs_set_modtime(path.data());


  FILE* fid = nullptr;
#ifdef _MSC_VER
    if (fopen_s(&fid, path.data(), "w") != 0)
      fid = nullptr;
#else
    fid = fopen(path.data(), "w");
#endif
  if(!fid){  FFS_UNLIKELY
    fs_print_error(path, "touch:fopen");
    return false;
  }

  if(fclose(fid) != 0){  FFS_UNLIKELY
    fs_print_error(path, "touch:fclose");
    return false;
  }

  if(fs_is_file(path.data()))  FFS_LIKELY
    return true;
  // is_file check because sometimes fclose() doesn't fail, but the file is not created

  fs_print_error(path, "touch");
  return false;
}

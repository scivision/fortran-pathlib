// verify functions handle empty input OK

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main(){

#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

    std::string_view e = "";

    if(!fs_as_posix(e).empty())
      err("as_posix");

    if(!fs_file_name(e).empty())
      err("file_name");

    if(!fs_stem("").empty())
      err("stem");

    if(!fs_join("", "").empty())
      err("join");

    if(!fs_suffix("").empty())
      err("suffix");

    if(!fs_with_suffix("", "").empty())
      err("with_suffix");

    if(fs_is_char_device(""))
      err("is_char_device");

    if(fs_is_reserved(""))
      err("is_reserved");

    if(fs_is_symlink(""))
      err("is_symlink");

    if(fs_create_symlink("", ""))
      err("create_symlink");

    if(fs_mkdir(""))
      err("mkdir");

    if(!fs_which("").empty())
      err("which");

    if(!fs_root("").empty())
      err("root");

    if(fs_exists(""))
      err("exists");

    if(fs_is_absolute(""))
      err("is_absolute");

    if(fs_is_dir(""))
      err("is_dir");

    if(fs_is_exe(""))
      err("is_exe");

    if(fs_is_file(""))
      err("is_file");

    if(fs_remove(""))
      err("remove");

    if(!fs_canonical("", false, false).empty())
      err("fs_canonical");

    if(fs_equivalent("", ""))
      err("equivalent");

    if(!fs_expanduser("").empty())
      err("expanduser");

    if(fs_copy_file("", "", false))
      err("copy_file");

    if(fs_touch(""))
      err("touch");


    if(fs_file_size(""))
      err("file_size");

    if(!fs_is_windows()){

      if(fs_space_available(""))
        err("space_available");

      if(fs_set_permissions("", 0, 0, 0))
        err("set_permissions");

    }

    if(fs_get_cwd().empty())
      err("get_cwd");

    if(fs_get_homedir().empty())
      err("get_homedir");

    std::cout << "OK: test_c_empty\n";

    return EXIT_SUCCESS;
}

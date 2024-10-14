// verify functions handle empty input OK

#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"

[[noreturn]] void err(std::string_view m){
    std::cerr << "ERROR: " << m << "\n";
    std::exit(EXIT_FAILURE);
}


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

    if(!Ffs::normal("").empty())
      err("Ffs::normal");

    if(!fs_file_name(e).empty())
      err("file_name");

    if(!fs_stem("").empty())
      err("stem");

    if(!Ffs::join("", "").empty())
      err("Ffs::join");

    if(!Ffs::suffix("").empty())
      err("Ffs::suffix");

    if(!Ffs::with_suffix("", "").empty())
      err("Ffs::with_suffix");

    if(fs_is_char_device(""))
      err("is_char_device");

    if(Ffs::is_reserved(""))
      err("Ffs::is_reserved");

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

    if(Ffs::canonical("", false, false))
      err("Ffs::canonical");

    if(Ffs::equivalent("", ""))
      err("Ffs::equivalent");

    if(!fs_expanduser("").empty())
      err("expanduser");

    if(fs_copy_file("", "", false))
      err("copy_file");

    if(Ffs::touch(""))
      err("Ffs::touch");


    if(fs_file_size(""))
      err("file_size");

    if(!fs_is_windows()){

      if(fs_space_available(""))
        err("space_available");

      if(fs_set_permissions("", 0, 0, 0))
        err("set_permissions");

    }

    if(!fs_get_cwd())
      err("get_cwd");

    if(fs_get_homedir().empty())
      err("get_homedir");

    std::cout << "OK: test_c_empty\n";

    return EXIT_SUCCESS;
}

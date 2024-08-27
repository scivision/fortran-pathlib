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

    if(!Ffs::as_posix("").empty())
      err("Ffs::as_posix");

    if(!Ffs::normal("").empty())
      err("Ffs::normal");

    if(!Ffs::file_name("").empty())
      err("Ffs::file_name");

    if(!Ffs::stem("").empty())
      err("Ffs::stem");

    if(!Ffs::join("", "").empty())
      err("Ffs::join");

    if(!Ffs::suffix("").empty())
      err("Ffs::suffix");

    if(!Ffs::with_suffix("", "").empty())
      err("Ffs::with_suffix");

    if(Ffs::is_char_device(""))
      err("Ffs::is_char_device");

    if(Ffs::is_reserved(""))
      err("Ffs::is_reserved");

    if(Ffs::is_symlink(""))
      err("Ffs::is_symlink");

    if(Ffs::create_symlink("", ""))
      err("Ffs::create_symlink");

    if(Ffs::mkdir(""))
      err("Ffs::mkdir");

    if(!Ffs::which("").empty())
      err("Ffs::which");

    if(!Ffs::root("").empty())
      err("Ffs::root");

    if(Ffs::exists(""))
      err("Ffs::exists");

    if(Ffs::is_absolute(""))
      err("Ffs::is_absolute");

    if(Ffs::is_dir(""))
      err("Ffs::is_dir");

    if(Ffs::is_exe(""))
      err("Ffs::is_exe");

    if(Ffs::is_file(""))
      err("Ffs::is_file");

    if(Ffs::remove(""))
      err("Ffs::remove");

    if(!Ffs::canonical("", false).empty())
      err("Ffs::canonical");

    if(Ffs::equivalent("", ""))
      err("Ffs::equivalent");

    if(!Ffs::expanduser("").empty())
      err("Ffs::expanduser");

    if(Ffs::copy_file("", "", false))
      err("Ffs::copy_file");

    if(Ffs::touch(""))
      err("Ffs::touch");


    if(Ffs::file_size(""))
      err("Ffs::file_size");

    if(!fs_is_windows()){

      if(Ffs::space_available(""))
        err("Ffs::space_available");

      if(Ffs::set_permissions("", 0, 0, 0))
        err("set_permissions");

    }

    if(Ffs::get_cwd().empty())
      err("get_cwd");

    if(Ffs::get_homedir().empty())
      err("get_homedir");

    std::cout << "OK: test_c_empty\n";

    return EXIT_SUCCESS;
}

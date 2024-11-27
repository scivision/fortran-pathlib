#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <string>

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

    const std::string ref(fs_devnull());

    std::string r;

    std::cout << "Begin test_reserved\n";

    r = fs_normal(ref);
    if (r != ref)
      err("normal");
    std::cout << "OK: normal(" << r << ")\n";

    bool b = fs_is_absolute(ref);
    if (fs_is_windows()){
      if(b) err("is_absolute");
    }
    else{
      if(!b) err("is_absolute");
    }
    std::cout << "OK: is_absolute(" << r << ")\n";

    b = fs_is_reserved(ref);
    if(fs_is_windows()){
      if(!b) err("is_reserved");
    }
    else{
      if(b) err("is_reserved");
    }
    std::cout << "OK: is_reserved(" << r << ")\n";

    if(fs_is_dir(ref))
      err("is_dir");
    std::cout << "OK: is_dir(" << r << ")\n";

    std::uintmax_t s = fs_file_size(ref);
    if(s != 0){
      std::cerr << "FAILED: file_size(" << ref << ") " << s << "\n";
      err("file_size");
    }
    std::cout << "OK: file_size() " << s << "\n";

    // omitted fs_space_available() since some systems don't handle NUL /dev/null well
    // e.g. Windows, macOS GCC, etc.


if(!fs_is_windows()){

    if(fs_is_exe(ref))
      err("is_exe");

    // NOTE: do not test
    //
    // create_directories(/dev/null)
    // remove(/dev/null)
    // create_symlink()
    // set_permissionss()
    //
    // since if testing with "root" privilidges,
    // it can make the system unusable until reboot!

    if(!fs_exists(ref))
      err("exists");
    std::cout << "OK: exists(" << r << ")\n";

    if(fs_is_file(ref))
      err("is_file");
    std::cout << "OK: is_file(" << r << ")\n";

    if(fs_canonical(ref.data(), false, true).empty())
      err("canonical");
    std::cout << "OK: canonical() " << ref << "\n";

    r = fs_relative_to(ref, ref);
    if(r != ".")
      err("relative_to");
} // fs_is_windows()

    r = fs_expanduser(ref);
    if(r != ref)
      err("expanduser");

    if(fs_copy_file(ref, ref, false))
      err("copy_file");
    std::cout << "OK: copy_file() " << ref << "\n";

    // touch is ambiguous on reserved, so omit

    if(fs_is_symlink(ref))
      err("is_symlink");
    std::cout << "OK: is_symlink() " << r << "\n";

    ok_msg("reserved C++");

    return EXIT_SUCCESS;
}

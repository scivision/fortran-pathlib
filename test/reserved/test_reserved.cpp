#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"

[[noreturn]] static void err(std::string_view msg, std::string_view buf){
  std::cerr << "FAILED: " << msg << " " << buf << "\n";
  exit(EXIT_FAILURE);
}

int main(void){

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
#ifdef HAVE_CXX_FILESYSTEM
    r = Ffs::normal(ref);
    if (r != ref)
      err("normal", r);
    std::cout << "OK: normal() " << r << "\n";
#endif

    bool b = fs_is_absolute(ref);
    if (fs_is_windows()){
      if(b) err("is_absolute", ref);
    }
    else{
      if(!b) err("is_absolute", ref);
    }
    std::cout << "OK: is_absolute " << r << "\n";

    if(fs_is_dir(ref))
      err("is_dir", ref);

if(!fs_is_windows()){

    if(fs_is_exe(ref))
      err("is_exe", ref);

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
      err("exists", ref);
    std::cout << "OK: exists() " << r << "\n";

    if(fs_is_file(ref))
      err("is_file", ref);

    // can cause exit code 409 (stack buffer overrun) on Windows for NUL
    auto s = fs_file_size(ref);
    if(s && s.value() != 0){
      std::cerr << "FAILED: file_size() " << s.value() << "\n";
      err("file_size", ref);
    }
    std::cout << "OK: file_size() " << s.value() << "\n";

#ifdef HAVE_CXX_FILESYSTME
    if(!Ffs::canonical(ref.data(), false, true))
      err("canonical", ref);
    std::cout << "OK: canonical() " << ref << "\n";

    r = Ffs::relative_to(ref, ref);
    if(r != ".")
      err("relative_to", r);
#endif
} // fs_is_windows()

    r = fs_expanduser(ref);
    if(r != ref)
      err("expanduser", r);

    if(fs_copy_file(ref, ref, false))
      err("copy_file", ref);
    std::cout << "OK: copy_file() " << ref << "\n";

    // touch is ambiguous on reserved, so omit

    if(fs_is_symlink(ref))
      err("is_symlink", ref);
    std::cout << "OK: is_symlink() " << r << "\n";

    std::cout << "PASS: test_reserved.cpp\n";

    return EXIT_SUCCESS;
}

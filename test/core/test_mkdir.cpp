#include <iostream>
#include <string>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main() {

    if(fs_mkdir(""))
      err("mkdir() should fail with empty path");


    // Get current working directory
    const std::string pwd = fs_get_cwd();
    if (pwd.empty())
      err("get_cwd() failed");

    std::cout << "test_mkdir: pwd " << pwd << std::endl;

    // Test mkdir with existing directory
    if(!fs_mkdir(pwd))
        err("mkdir() should not fail with existing directory");

    // Test mkdir with full path
    std::string p1 = pwd + "/test-filesystem-dir1";
    if(fs_is_dir(p1))
      fs_remove(p1);

    if(!fs_mkdir(p1))
      err("mkdir() failed with full path " + p1);

    if (!fs_is_dir(p1))
        err("mkdir() failed to create directory " + p1);

    std::cout << "PASS: mkdir full-path" << std::endl;

    fs_remove(p1);
    std::cout << "PASS: mkdir existing" << std::endl;

    // Test mkdir with relative path
    p1 = "test-filesystem-dir/hello";
    if(!fs_mkdir(p1))
      err("test_mkdir: relative mkdir(" + p1 + ")");
    if(!fs_is_dir(p1))
      err("test_mkdir: relative is_dir(" + p1 + ")");

    std::cout << "PASS: mkdir relative\n";

    p1 = pwd + "/日本語";
    if(fs_exists(p1))
      fs_remove(p1);

    if(!fs_mkdir(p1)){
      if(fs_is_windows() && fs_backend() == "C"){
        std::cerr << "mkdir non-ASCII fails on Windows C backend due to CreateDirectoryA\n";
        return EXIT_SUCCESS;
      } else
        err("test_mkdir: non-ASCII mkdir(" + p1 + ")");
    }
    if(!fs_is_dir(p1))
      err("test_mkdir: non-ASCII is_dir(" + p1 + ")");

    std::cout << "PASS: mkdir non-ASCII " << p1 << "\n";

    if(!fs_set_cwd(p1) || fs_get_cwd() != p1)
      err("test_mkdir: set_cwd " + p1);

    std::cout << "PASS: set_cwd " << p1 << "\n";

    std::cout << "OK: mkdir\n";

    return EXIT_SUCCESS;
}

#include <iostream>
#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

#include <chrono>
#include <thread>


void check_touch(std::string_view in)
{

  std::string fn(in);

  std::cout << "touch(" << fn << ") exists? " << fs_is_file(fn) << "\n";
  if(!fs_touch(fn))
    err("touch failed " + fn);

  std::time_t t0 = fs_get_modtime(fn);
  if(t0 <= 0)
      err("get_modtime failed " + fn);

  std::cout << "PASSED: touch absolute path: " << fn << " mod time (posix): " << t0 << "\n";

  std::cout << "touch relative path, cwd: " << fs_get_cwd() << "\n";
  fn = "test_fileop.h5";
  if(!fs_touch(fn))
      err("touch failed " + fn);

  t0 = fs_get_modtime(fn);
  if(t0 <= 0)
      err("get_modtime failed " + fn);

  if(!fs_set_modtime(fn))
      err("set_modtime failed " + fn);

  std::time_t t1 = fs_get_modtime(fn);
  if(t1 < t0)
      err("set_modtime failed " + fn);

  if(fs_set_modtime("not-exist-tile"))
      err("set_modtime should have failed on non existing file");

  std::cout << "OK: touch. modtime: " <<  std::ctime(&t0) << "\n"; // NOSONAR
}


int main(
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
int argc,
char* argv[])
{
  const std::string dir = fs_parent(argv[0]);

  check_touch(dir + "/test_fileop.h5");

  if(fs_is_mingw() && fs_backend() != "<filesystem>")
    std::cerr << "WARNING: skipping symlink tests on MinGW with " << fs_backend() << "\n";
  else
    check_touch(dir + "/日本語.txt");

  return 0;
}

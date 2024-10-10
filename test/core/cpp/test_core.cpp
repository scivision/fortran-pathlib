#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"

[[noreturn]] void err(std::string_view m){
    std::cerr << "ERROR: " << m << "\n";
    std::exit(EXIT_FAILURE);
}

void test_as_posix(){

  std::string p;

  if(!Ffs::as_posix(p).empty())
    err("test_as_posix: " + p);

  if(fs_is_windows()){
    p = "a\\b";
    if(Ffs::as_posix(p) != "a/b")
      err("test_as_posix: " + Ffs::as_posix(p) + " != a/b");

    p = "C:\\my\\path";
    if(Ffs::as_posix(p) != "C:/my/path")
      err("test_as_posix: " + p);
  }

  std::cout << "OK: as_posix\n";
}

void test_filename()
{

if(fs_file_name("") != "")
  err("filename empty: " + fs_file_name(""));

std::cout << "PASS:filename:empty\n";

if (fs_file_name("a/b/c") != "c")
  err("file_name failed: " + fs_file_name("a/b/c"));

if (fs_file_name("a") != "a")
  err("file_name idempotent failed: " + fs_file_name("a"));

if(fs_file_name("file_name") != "file_name")
  err("file_name plain filename: " + fs_file_name("file_name"));

auto cwd = fs_get_cwd();
if(!cwd)
  err("file_name cwd");
if(std::string nr = fs_file_name(fs_root(cwd.value())); !nr.empty())
  err("file_name root: " + nr);

if(fs_file_name(".file_name") != ".file_name")
  err("file_name leading dot filename: " + fs_file_name(".file_name"));

if(fs_file_name("./file_name") != "file_name")
  err("file_name leading dot filename cwd: " + fs_file_name("./file_name"));

if(fs_file_name("file_name.txt") != "file_name.txt")
  err("file_name leading dot filename w/ext");

if(fs_file_name("./file_name.txt") != "file_name.txt")
  err("file_name leading dot filename w/ext and cwd");

if(fs_file_name("../file_name.txt") != "file_name.txt")
  err("file_name leading dot filename w/ext up " + fs_file_name("../file_name.txt"));

if(fs_is_windows() && fs_file_name("c:\\my\\path") != "path")
  err("file_name windows: " + fs_file_name("c:\\my\\path"));

if(fs_is_windows() && fs_pathsep() != ';')
  err("pathsep windows");
if(!fs_is_windows() && fs_pathsep() != ':')
  err("pathsep unix");

}


int main() {

#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

  test_as_posix();
  test_filename();

  return EXIT_SUCCESS;
}

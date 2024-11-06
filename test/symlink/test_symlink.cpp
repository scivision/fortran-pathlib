#include <iostream>
#include "ffilesystem.h"
#include <cstdlib>



int main([[maybe_unused]] int argc, char* argv[]) {

  std::string tgt_dir = fs_parent(argv[0]);
  std::string tgt = tgt_dir + "/test.txt";

  if (fs_exists(tgt)) {
    std::cout << "deleting old target " << tgt << "\n";
    fs_remove(tgt);
  }

  if(!fs_touch(tgt)){
    std::cerr << "ERROR: touch(" << tgt << ") failed\n";
    return EXIT_FAILURE;
  }
  std::cout << "created target file " << tgt << "\n";

  if (!fs_is_file(tgt)) {
    std::cerr << "ERROR: is_file(" << tgt << ") should be true for existing regular file target\n";
    return EXIT_FAILURE;
  }

  std::string link = tgt_dir + "/test.link";
  std::string linko = tgt_dir + "/test_oo.link";
  std::string link_dir = tgt_dir + "/my_link.dir";

  if(fs_create_symlink(tgt, "")){
    std::cerr << "ERROR: create_symlink() should fail with empty link\n";
    return EXIT_FAILURE;
  }
  std::cout << "PASSED: create_symlink: empty link\n";

  if (fs_is_symlink(tgt)) {
    std::cerr << "is_symlink() should be false for non-symlink file: " << tgt << "\n";
    return EXIT_FAILURE;
  }

  if(fs_create_symlink("", link)){
    std::cerr << "ERROR: create_symlink() should fail with empty target\n";
    return EXIT_FAILURE;
  }
  std::cout << "PASSED: create_symlink: empty target\n";


  if (fs_is_symlink(link)) {
    std::cout << "deleting old symlink " << link << "\n";
    fs_remove(link);
  }
  if(!fs_create_symlink(tgt, link)){
    std::cerr << "ERROR: create_symlink() failed\n";
    return EXIT_FAILURE;
  }
  std::cout << "PASSED: create_symlink " << link << "\n";


  std::string rtgt = fs_read_symlink(link);
  if (rtgt != tgt) {
    std::cerr << "read_symlink() failed: " << rtgt << " != " << tgt << "\n";
    return EXIT_FAILURE;
  }
  std::cout << "PASSED: read_symlink " << rtgt << " == " << tgt << "\n";

  std::string ctgt = fs_canonical(link, true, false);
  if (ctgt != tgt) {
    std::cerr << "canonical() on symlink failed: " << ctgt << " != " << tgt << "\n";
    return EXIT_FAILURE;
  }
  std::cout << "PASSED: canonical() on symlink " << ctgt << " == " << tgt << "\n";

  rtgt = fs_read_symlink(tgt);
  if(!rtgt.empty()){
    std::cerr << "read_symlink() should return empty string for non-symlink file: " << rtgt << "\n";
    return EXIT_FAILURE;
  }

  rtgt = fs_read_symlink("not-exist-file");
  if (!rtgt.empty()) {
    std::cerr << "read_symlink() should return empty string for non-existent file: " << rtgt << "\n";
    return EXIT_FAILURE;
  }


  if (fs_is_symlink(linko)) {
    std::cout << "deleting old symlink " << linko << "\n";
    fs_remove(linko);
  }
  if(!fs_create_symlink(tgt, linko)){
    std::cerr << "ERROR: create_symlink() failed\n";
    return EXIT_FAILURE;
  }
  std::cout << "PASSED: created symlink " << linko << "\n";

  if (fs_is_symlink(link_dir)) {
    std::cout << "deleting old symlink " << link_dir << "\n";
    fs_remove(link_dir);
  }
  if(!fs_create_symlink(tgt_dir, link_dir)){
    std::cerr << "ERROR: create_symlink() failed\n";
    return EXIT_FAILURE;
  }

  if (fs_is_symlink(tgt)) {
    std::cerr << "ERROR: is_symlink(" << tgt << ") should be false for non-symlink target\n";
    return EXIT_FAILURE;
  }

  if (!fs_is_symlink(link)) {
    std::cerr << "ERROR: is_symlink(" << link << ") should be true\n";
    return EXIT_FAILURE;
  }

  if (!fs_is_file(link)) {
    std::cerr << "ERROR: is_file(" << link << ") should be true for existing regular file target " << tgt << "\n";
    return EXIT_FAILURE;
  }

  std::cout << "PASSED: test_symlink: file\n";

  if (fs_is_symlink(tgt_dir)) {
    std::cerr << "is_symlink() should be false for non-symlink dir\n";
    return EXIT_FAILURE;
  }

  if (!fs_is_dir(link_dir)) {
    std::cerr << "ERROR: is_dir(" << link_dir << ") should be true for existing regular dir\n";
    return EXIT_FAILURE;
  }

  if (!fs_is_symlink(link_dir)) {
    std::cerr << "ERROR: is_symlink() should be true for symlink dir: " << link_dir << "\n";
    return EXIT_FAILURE;
  }

  std::cout << "OK: filesystem symbolic links\n";

  return 0;
}

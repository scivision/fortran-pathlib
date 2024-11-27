#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int
main(
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
int argc, char* argv[])
{

  std::string_view in = argv[0];

  const std::string tgt_dir = fs_parent(in);
  const std::string tgt = tgt_dir + "/test_symlink.txt";

  if(!fs_touch(tgt))
    err("touch(" + tgt + ") failed");

  std::cout << "target file " << tgt << "\n";

  if (!fs_is_file(tgt))
    err("is_file(" + tgt + ") should be true for existing regular file target");

  const std::string link = tgt_dir + "/test.link";
  const std::string linko = tgt_dir + "/test_oo.link";
  const std::string link_dir = tgt_dir + "/my_link.dir";

  if(fs_create_symlink(tgt, ""))
    err("create_symlink() should fail with empty link");

  std::cout << "PASSED: create_symlink: empty link\n";

  if (fs_is_symlink(tgt))
    err("is_symlink() should be false for non-symlink file: " + tgt);

  if(fs_create_symlink("", link))
    err("create_symlink() should fail with empty target");

  std::cout << "PASSED: create_symlink: empty target\n";


  if (fs_is_symlink(link))
    fs_remove(link);

  if(!fs_create_symlink(tgt, link))
    err("create_symlink() failed");

  std::cout << "PASSED: create_symlink " << link << "\n";


  std::string rtgt = fs_read_symlink(link);
  if (rtgt != tgt)
    err("read_symlink() failed: " + rtgt + " != " + tgt);

  std::cout << "PASSED: read_symlink " << rtgt << " == " << tgt << "\n";

  std::string ctgt = fs_canonical(link, true, false);
  if(ctgt.empty())
    err("canonical() is empty");

  // Cygwin will have /cygdrive/c and /home/ as roots
  if (!fs_is_cygwin() && ctgt != tgt)
    err("canonical() on symlink failed: " + ctgt + " != " + tgt);

  std::cout << "PASSED: canonical() on symlink " << ctgt << " == " << tgt << "\n";

  rtgt = fs_read_symlink(tgt);
  if(!rtgt.empty())
    err("read_symlink() should be empty string for non-symlink file: " + rtgt);

  rtgt = fs_read_symlink("not-exist-file");
  if (!rtgt.empty())
    err("read_symlink() should be empty string for non-existent file: " + rtgt);

  if (fs_is_symlink(linko))
    fs_remove(linko);

  if(!fs_create_symlink(tgt, linko))
    err("create_symlink() failed");

  std::cout << "PASSED: created symlink " << linko << "\n";

  if (fs_is_symlink(tgt_dir))
    err("is_symlink() should be false for non-symlink dir");

  if (fs_is_symlink(link_dir)) {
    std::cout << "deleting old symlink " << link_dir << "\n";
    fs_remove(link_dir);
  }
  if(!fs_create_symlink(tgt_dir, link_dir))
    err("create_symlink() failed");

  if (fs_is_symlink(tgt))
    err("is_symlink(" + tgt + ") should be false for non-symlink target");

  if (!fs_is_symlink(link))
    err("is_symlink(" + link + ") should be true");

  if (!fs_is_file(link))
    err("is_file(" + link + ") should be true for existing regular file target " + tgt);

  if (!fs_is_dir(link_dir))
    err("is_dir(" + link_dir + ") should be true for existing regular dir");

  if (!fs_is_symlink(link_dir))
    err("is_symlink() should be true for symlink dir: " + link_dir);

  std::cout << "PASSED: test_symlink: file / directory\n";

  if(fs_is_windows())
    std::cerr << "SKIP: non-ASCII on Windows create_symlink currently broken with std::filesystem or C backend\n";
  else {
    const std::string cwd = fs_get_cwd();
    const std::string j = cwd + "/日本語_create_symlink.lnk";

    if (fs_is_symlink(j)){
      std::cout << "deleting old symlink " << j << "\n";
      fs_remove(j);
    } else
      std::cout << "did not detect any existing symlink " << j << "\n";

    if (!fs_create_symlink(tgt, j))
      err("create_symlink() failed with non-ASCII link");

    std::cout << "created symlink " << j << "\n";

    if(!fs_is_symlink(j))
      err("is_symlink() should be true for non-ASCII link");

    std::cout << "PASSED: create_symlink() with non-ASCII link " << j << "\n";
  }

  ok_msg("create_symlink C++");

  return 0;
}

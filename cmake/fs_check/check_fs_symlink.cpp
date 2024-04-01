// Check that filesystem is capable of symbolic links with this compiler.
#include <iostream>
#include <cstdlib>

#include <filesystem>

static_assert(__cpp_lib_filesystem, "No C++ filesystem support");

[[noreturn]] void err(std::string_view m){
    std::cerr << "ERROR: " << m << "\n";
    std::exit(EXIT_FAILURE);
}


int main(int argc, char **argv){

if(argc < 2)
  err("missing argument for target");

auto tgt = std::filesystem::canonical(argv[1]);

if(!std::filesystem::is_regular_file(tgt))
  err("target " + tgt.generic_string() + " is not a regular file");

auto lnk = tgt.parent_path() / "test.lnk";

if(!std::filesystem::exists(lnk)) {
  std::filesystem::create_symlink(tgt, lnk);
  std::cout << "created symlink: " << lnk << "\n";
}
auto l = std::filesystem::symlink_status(lnk);

if(!std::filesystem::exists(l))
  err("symlink not created: " + lnk.generic_string());

if(!std::filesystem::is_symlink(l))
  err("is not a symlink: " + lnk.generic_string());

return EXIT_SUCCESS;
}

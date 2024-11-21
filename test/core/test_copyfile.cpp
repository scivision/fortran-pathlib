#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"

int main() {
    // use full path for clarity of user messages

    const std::string cwd = fs_get_cwd();

    const std::string s1 = cwd + "/dummy.txt";

    std::string s2 = cwd;
    if(fs_is_mingw() && fs_backend() == "<filesystem>")
    // bug in MinGW and CopyFileA with non-ASCII
      s2 += "/copy.txt";
    else
      s2 += "/日本語.txt";

    const std::string t1 = "及せゃ市人購ゅトてへ投際ト点吉で速流つ今日";
    std::string t2;

    // Write to the first file
    std::ofstream ofs(s1);
    if (!ofs)
      err("could not open file for writing " + s1);
    ofs << t1;
    ofs.close();

    auto iref = fs_file_size(s1);
    if(iref == 0)
      err("input file size is zero " + s1);

    // Copy the file
    bool ok = fs_copy_file(s1, s2, true);
    if (!fs_is_file(s2))
        err("did not detect file after copy " + s2);
    if (!ok)
      err("copy_file ok logical is false despite success of copy ");

    auto i64 = fs_file_size(s2);
    if(i64 == 0)
      err("file size is zero after copy " + s2);

    std::cout << "file sizes (bytes) original, copy: " << iref << " " << i64 << "\n";

    if (i64 != iref)
      err("file size mismatch after copy " + s1 + " " + s2);

    // Read from the copied file
    std::ifstream ifs(s2);
    if (!ifs)
      err("could not open file for reading " + s2);

    std::getline(ifs, t2);
    ifs.close();

    // Check file contents

    std::cout << "text lengths (bytes) original, copy: " << t1.length() << " " << t2.length() << "\n";
    std::cout << "File text contents:\n" << t1 << "\nCopy contents:\n" << t2 << "\n";

    if (t1 != t2) {
        std::cerr << "file content mismatch after copy:\n" << s1 << " => " << s2 << "\n";
        return EXIT_FAILURE;
    }

    // copy empty file
    const std::string s3 = cwd + "/empty.txt";
    const std::string s4 = cwd + "/empty_copy.txt";

    fs_touch(s3);
    ok = fs_copy_file(s3, s4, true);
    if (!fs_is_file(s4))
      err("did not detect file after copy " + s4);
    if (!ok)
      err("copy_file ok logical is false despite success of copy ");

    i64 = fs_file_size(s4);
    if(i64 != 0)
      err("empty file size is not zero after copy " + s4);

    std::cout << "OK: C++ copyfile\n";

    return EXIT_SUCCESS;
}

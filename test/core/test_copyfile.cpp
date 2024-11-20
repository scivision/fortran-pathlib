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
    const std::string s2 = cwd + "/日本語.txt";
    const std::string t1 = "及せゃ市人購ゅトてへ投際ト点吉で速流つ今日";
    std::string t2;

    // Write to the first file
    std::ofstream ofs(s1);
    if (!ofs)
      err("could not open file for writing " + s1);
    ofs << t1;
    ofs.close();

    // Copy the file
    bool ok = fs_copy_file(s1, s2, true);
    if (!fs_exists(s2))
        err("did not detect file after copy " + s2);

    if (!ok)
      err("copy_file ok logical is false despite success of copy ");

    // Check file sizes
    auto iref = fs_file_size(s1);
    auto i64 = fs_file_size(s2);
    if (i64 != iref)
      err("file size mismatch after copy " + s1 + " " + s2 + " " + std::to_string(iref) + " " + std::to_string(i64));

    // Read from the copied file
    std::ifstream ifs(s2);
    if (!ifs)
      err("could not open file for reading " + s2);

    std::getline(ifs, t2);
    ifs.close();

    // Check file contents

    std::cout << "File text contents:\n" << t1 << "\nCopy contents:\n" << t2 << "\n" << "file lengths " << t1.length() << " " << t2.length() << "\n";

    if (t1 != t2) {
        std::cerr << "file content mismatch after copy:\n" << s1 << " => " << s2 << "\n";
        return EXIT_FAILURE;
    }

    std::cout << "OK: C++ copyfile\n";

    return EXIT_SUCCESS;
}

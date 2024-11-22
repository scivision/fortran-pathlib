#include <iostream>
#include "ffilesystem.h"
#include <cstdlib>

int main() {
    std::string in = "rel";
    std::string base, ref, out;
    const std::string cwd = fs_get_cwd();

    int err = 0;

    if (fs_is_windows()) {
        base = "j:/foo";
        ref = "j:/foo/rel";
    } else {
        base = "/foo";
        ref = "/foo/rel";
    }

    out = fs_absolute(in, base, false);
    if (out.empty()) {
        std::cerr << "absolute() has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "Mismatch: absolute(" << in << ", " << base << ") " << out << " != " << ref << "\n";
        err++;
    }

    // relative path alone
    ref = cwd + "/" + in;
    out = fs_absolute(in, false);
    if (out.empty()) {
        std::cerr << "absolute(" << in << ") has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "absolute(" << in << ") = " << out << " != " << ref << "\n";
        err++;
    }

    ref = cwd + "/./" + in;
    out = fs_absolute("./" + in, false);
    if (out.empty()) {
        std::cerr << "test 3: absolute(./" << in << ") has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "test 3:  absolute(./" << in << "): " << out << " != " << ref << "\n";
        err++;
    }

    ref = cwd + "/../" + in;
    out = fs_absolute("../" + in, false);
    if (out.empty()) {
        std::cerr << "test 4: absolute(../" << in << ") has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "test 4: absolute(../" << in << "): " << out << " != " << ref << "\n";
        err++;
    }

    // relative path, empty base
    ref = cwd + "/" + in;
    out = fs_absolute(in, "", false);
    if (out.empty()) {
        std::cerr << "test 5: absolute(" << in << ", '') has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "test 5: absolute(" << in << ", ''): " << out << " != " << ref << "\n";
        err++;
    }

    // relative path, '.' base
    ref = cwd + "/./" + in;
    out = fs_absolute(in, ".", false);
    if (out.empty()) {
        std::cerr << "test 6: absolute(" << in << ", '.') has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "test 6: absolute(" << in << ", '.'): " << out << " != " << ref << "\n";
        err++;
    }

    // relative path, '..' base
    ref = cwd + "/../" + in;
    out = fs_absolute(in, "..", false);
    if (out.empty()) {
        std::cerr << "test 7: absolute(" << in << ", '..') has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "test 7: absolute(" << in << ", '..'): " << out << " != " << ref << "\n";
        err++;
    }

    // empty path, relative base
    ref = cwd + "/" + in;
    out = fs_absolute("", in, false);
    if (out.empty()) {
        std::cerr << "absolute() has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "Mismatch: absolute('', " << in << ") " << out << " != " << in << "\n";
        err++;
    }

    // empty in
    ref = cwd;
    out = fs_absolute("", false);
    if (out.empty()) {
        std::cerr << "absolute('') has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "absolute(''): " << out << " != " << ref << "\n";
        err++;
    }

    out = fs_absolute("", "", false);
    if (out.empty()) {
        std::cerr << "absolute('', '') has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "absolute('', ''): " << out << " != " << ref << "\n";
        err++;
    }

    // base only
    ref = cwd + "/" + in;
    out = fs_absolute("", in, false);
    if (out.empty()) {
        std::cerr << "absolute('', path) has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "absolute('', " << in << "): " << out << " != " << ref << "\n";
        err++;
    }

    // non-ASCII
    in = "日本語";
    ref = cwd + "/" + in;
    out = fs_absolute(in, false);
    if (out.empty()) {
        std::cerr << "absolute(" << in << ") has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "absolute(" << in << "): " << out << " != " << ref << "\n";
        err++;
    }

    // space
    in = "some space here";
    ref = cwd + "/" + in;
    out = fs_absolute(in, false);
    if (out.empty()) {
        std::cerr << "absolute(" << in << ") has empty output\n";
        err++;
    }
    if (out != ref) {
        std::cerr << "absolute(" << in << "): " << out << " != " << ref << "\n";
        err++;
    }

    if(err){
        std::cerr << "FAILED: absolute C++ " << err << " tests.\n";
        return EXIT_FAILURE;
    }

    std::cout << "OK: absolute C++\n";

    return EXIT_SUCCESS;
}

#include <iostream>
#include "ffilesystem.h"
#include "ffilesystem_test.h"
#include <cstdlib>

int main() {
    std::string in = "rel";
    std::string base, ref, out;

    if (fs_is_windows()) {
        base = "j:/foo";
        ref = "j:/foo/rel";
    } else {
        base = "/foo";
        ref = "/foo/rel";
    }

    out = fs_absolute(in, base, false);
    if (out.empty())
        err("absolute() has empty output");
    if (out != ref)
        err("Mismatch: absolute(" + in + ", " + base + ") " + out + " != " + ref);

// relative path in
    ref = fs_get_cwd() + "/" + in;
    out = fs_absolute(in, false);
    if (out.empty())
        err("absolute(" + in + ") has empty output");
    if (out != ref)
        err("absolute(" + in + ") = " + out + " != " + ref);

    out = fs_absolute(in, "", false);
    if (out.empty())
        err("absolute(" + in + ", '') has empty output");
    if (out != ref)
        err("absolute(" + in + ", ''): " + out + " != " + ref);

    out = fs_absolute("", in, false);
    if (out.empty())
        err("absolute() has empty output");
    if (out != ref)
        err("Mismatch: absolute('', " + in + ") " + out + " != " + in);

// empty in
    ref = fs_get_cwd();
    out = fs_absolute("", false);
    if (out.empty())
        err("absolute('') has empty output");
    if (out != ref)
        err("absolute(''): " + out + " != " + ref);

    out = fs_absolute("", "", false);
    if (out.empty())
        err("absolute('', '') has empty output");
    if (out != ref)
        err("absolute('', ''): " + out + " != " + ref);

// base only
    ref = fs_get_cwd() + "/" + in;
    out = fs_absolute("", in, false);
    if (out.empty())
        err("absolute('', path) has empty output");
    if (out != ref)
        err("absolute('', "  + in + "): " + out + " != " + ref);

// non-ASCII
    in = "日本語";
    ref = fs_get_cwd() + "/" + in;
    out = fs_absolute(in, false);
    if (out.empty())
        err("absolute(" + in + ") has empty output");
    if (out != ref)
        err("absolute(" + in + "): " + out + " != " + ref);

// space
    in = "some space here";
    ref = fs_get_cwd() + "/" + in;
    out = fs_absolute(in, false);
    if (out.empty())
        err("absolute(" + in + ") has empty output");
    if (out != ref)
        err("absolute(" + in + "): " + out + " != " + ref);

    std::cout << "OK: absolute C++\n";

    return EXIT_SUCCESS;
}

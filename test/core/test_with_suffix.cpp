#include "ffilesystem.h"
#include "ffilesystem_test.h"

#include <cstdlib>
#include <iostream>

int main() {
    std::string r;

    r = fs_with_suffix("", ".h5");
    if (r != ".h5")
        err("with_suffix empty path: " + r);

    r = fs_with_suffix("foo.h5", "");
    if(r != "foo")
        err("with_suffix empty suffix: " + r);


    r = fs_with_suffix(".foo.h5", ".txt");
    if(r != ".foo.txt")
        err("with_suffix: " + r);

    r = fs_with_suffix(".h5", "");
    if(r != ".h5")
        err("with_suffix: " + r);

    r = fs_with_suffix(".h5", ".h5");
    if (r != ".h5.h5")
        err("with_suffix(.h5,.h5) " + r);

    r = fs_with_suffix("a//b///c///", ".h5");
    if(r != "a/b/c/.h5")
        err("with_suffix: " + r);

    r = fs_with_suffix("c:/a/hi.nc", ".h5");
    if (r != "c:/a/hi.h5")
        err("with_suffix c:/a/hi.nc to .h5: " + r);

    r = fs_with_suffix("my/file.h5", ".hdf5");
    if(r != "my/file.hdf5")
        err("with_suffix: " + r);

    r = fs_with_suffix("a/boo", ".h5");
    if(r != "a/boo.h5")
        err("with_suffix: " + r);

    r = fs_with_suffix("boo", ".h5");
    if(r != "boo.h5")
        err("with_suffix: " + r);

    // this is an odd case that wouldn't be expected to be used
    r = fs_with_suffix("a/b/c.d/", ".hdf5");
    if(r != "a/b/c.d/.hdf5")
        err("with_suffix: " + r);

    r = fs_with_suffix("dir.h5/", ".hdf5");
    if(r != "dir.h5/.hdf5")
        err("with_suffix: " + r);

    std::cout << "OK: with_suffix\n";

    return EXIT_SUCCESS;
}

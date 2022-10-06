#include <iostream>
#include <cstdlib>
#include <string>

#include "ffilesystem.h"


int test_exe_path(char* argv[]){

  char binpath[MAXP], bindir[MAXP], p[MAXP];

  fs_exe_path(binpath, MAXP);

  std::string bp(binpath);

  if (bp.find(argv[2]) == std::string::npos) {
    std::cerr << "ERROR:test_binpath: exe_path not found correctly: " << binpath << std::endl;
    return 1;
  }

  size_t L = fs_exe_dir(bindir, MAXP);
  if(L == 0){
    std::cerr << "ERROR:test_binpath: exe_dir not found correctly: " << bindir << std::endl;
    return 1;
  }
  fs_parent(binpath, p, MAXP);

  if(!fs_equivalent(bindir, p)){
    std::cerr << "ERROR:test_binpath: exe_dir and parent(exe_path) should be equivalent: " << bindir << " " << p << std::endl;
    return 1;
  }

  std::cout << "OK: exe_path: " << binpath << std::endl;
  std::cout << "OK: exe_dir: " << bindir << std::endl;
  return 0;
}

int test_lib_path(int argc, char* argv[]){

  char binpath[MAXP], bindir[MAXP];

  if(argc < 2){
    std::cerr << "need argument 0 for static or 1 for shared.  Got: " << argv[1] << std::endl;
    return 1;
  }

  int shared = std::stoi(argv[1]);

  size_t L = fs_lib_path(binpath, MAXP);
  size_t L2 = fs_lib_dir(bindir, MAXP);

  if(!shared) {
    if (L != 0 || L2 != 0) {
      std::cerr << "ERROR:test_binpath_cpp: lib_path and lib_dir should be empty length 0: " << binpath << " " << L << std::endl;
      return 1;
    }
    std::cout << "SKIPPED: lib_path: due to static library" << std::endl;
    return 0;
  }

#ifdef __APPLE__
#define name "ffilesystem.dylib"
#elif defined(_WIN32)
#define name "ffilesystem.dll"
#else
#define name "libffilesystem.so"
#endif

  std::string bp(binpath);

  if(bp.find(name) == std::string::npos){
    std::cerr << "ERROR:test_binpath: lib_path not found correctly: " << binpath << " with name " << name << std::endl;
    return 1;
  }

  char parent[MAXP];

  fs_parent(binpath, parent, MAXP);

  if(!fs_equivalent(bindir, parent)){
    std::cerr << "ERROR:test_binpath: lib_dir and parent(lib_path) should be equivalent: " << bindir << " " << parent << std::endl;
    return 1;
  }

  std::cout << "OK: lib_path: " << binpath << std::endl;
  std::cout << "OK: lib_dir: " << bindir << std::endl;
  return 0;
}

int main(int argc, char* argv[]){

  int i = test_exe_path(argv);

  i += test_lib_path(argc, argv);

  return i;
}

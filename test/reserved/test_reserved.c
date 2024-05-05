#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "ffilesystem.h"

static void err(const char* msg, char* buf){
  fprintf(stderr, "ERROR: %s %s\n", msg, buf);

  free(buf);
  exit(EXIT_FAILURE);
}

int main(void){

#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

    char s[10];
    strcpy(s, fs_devnull());

    const char* ref = fs_devnull();

    const size_t maxp = fs_get_max_path();

    char* p = (char*)malloc(maxp * sizeof(char));
    if(!p)
      err("malloc", p);

    printf("Begin test_reserved\n");

    fs_normal(s, p, maxp);
    if (strcmp(p, ref) != 0)
      err("normal", p);
    printf("OK: normal(%s)\n", p);

    bool b = fs_is_absolute(s);
    if (fs_is_windows()){
      if(b) err("is_absolute", p);
    }
    else{
      if(!b) err("is_absolute", p);
    }
    printf("OK: is_absolute(%s)\n", ref);

    if(fs_is_dir(s))
      err("is_dir", p);

if(!fs_is_windows()){

    if(fs_is_exe(s))
      err("is_exe", p);

    // NOTE: do not test
    //
    // create_directories(/dev/null)
    // remove(/dev/null)
    // create_symlink()
    // set_permissionss()
    //
    // since if testing with "root" privilidges,
    // it can make the system unusable until reboot!

    if(!fs_exists(s))
      err("exists", p);
    printf("OK: exists(%s)\n", ref);

    if(fs_is_file(s))
      err("is_file", p);

    if(fs_canonical(s, false, p, maxp) == 0)
      err("canonical", p);
    printf("OK: canonical(%s)\n", p);

    fs_relative_to(s, s, p, maxp);
    if(strcmp(p, ".") != 0)
      err("relative_to", p);
}

    fs_expanduser(s, p, maxp);
    if(strcmp(p, ref) != 0)
      err("expanduser", p);

    if(fs_copy_file(s, s, false))
      err("copy_file", p);
    printf("OK: copy_file(%s)\n", s);

    // touch is ambiguous on reserved, so omit

    if(fs_file_size(s) != 0)
      err("file_size", p);
    printf("OK: file_size(%s)\n", ref);

    if(fs_is_symlink(s))
      err("is_symlink", p);
    printf("OK: is_symlink(%s)\n", s);

    printf("PASS: test_reserved.cpp\n");

    free(p);
    return EXIT_SUCCESS;

}

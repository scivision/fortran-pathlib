#include <stdbool.h>
#include <stdio.h>  // fopen, fclose, fread, fwrite

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <copyfile.h>
#endif

#include "ffilesystem.h"


bool fs_copy_file(const char* source, const char* dest, bool overwrite)
{

  if(overwrite && fs_is_file(dest) && !fs_remove(dest))
    fprintf(stderr, "ERROR:ffilesystem:copy_file: could not remove existing destination file %s\n", dest);

#if defined(_WIN32)
    if(!CopyFile(source, dest, true)){
      fprintf(stderr, "ERROR:ffilesystem:copy_file: could not copy file %s to %s\n", source, dest);
      return false;
    }
#elif defined(__APPLE__) && defined(__MACH__)
  /* copy-on-write file
  * based on kwSys:SystemTools:CloneFileContent
  * https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/copyfile.3.html
  * COPYFILE_CLONE is a 'best try' flag, which falls back to a copy if the clone fails.
  */
  if(copyfile(source, dest, NULL, COPYFILE_METADATA | COPYFILE_CLONE) < 0){
    fprintf(stderr, "ERROR:ffilesystem:copy_file: could not clone file %s to %s\n", source, dest);
    return false;
  }
#else
    // https://stackoverflow.com/a/29082484
    const int bufferSize = 4096;
    char buf[bufferSize];
    FILE *rid = fopen(source, "r");
    FILE *wid = fopen(dest, "w");

    if (rid == NULL || wid == NULL) {
      fprintf(stderr, "ERROR:ffilesystem:copy_file: could not open file %s or %s\n", source, dest);
      return false;
    }

    while (!feof(rid)) {
      size_t bytes = fread(buf, 1, sizeof(buf), rid);
      if (bytes)
        fwrite(buf, 1, bytes, wid);
    }

    fclose(rid);
    fclose(wid);
#endif

  return fs_is_file(dest);
}

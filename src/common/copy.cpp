#include "ffilesystem.h"

#include <iostream>
#include <system_error>
#include <string>

#ifndef HAVE_CXX_FILESYSTEM
#include <cstdio>  // fopen, fclose, fread, fwrite

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <copyfile.h>
#endif
#endif


bool fs_copy_file(std::string_view source, std::string_view dest, bool overwrite)
{
#ifdef HAVE_CXX_FILESYSTEM
  auto opt = std::filesystem::copy_options::none;
  if (overwrite)
    opt = std::filesystem::copy_options::overwrite_existing;
// WORKAROUND: Windows MinGW GCC 11..13, Intel oneAPI Linux: bug with overwrite_existing failing on overwrite

  if(overwrite && fs_is_file(dest) && !fs_remove(dest)) FFS_UNLIKELY
    std::cerr << "ERROR:Ffs::copy_file: could not remove existing destination file: " << dest << "\n";

  std::error_code ec;
  if(std::filesystem::copy_file(source, dest, opt, ec) && !ec) FFS_LIKELY
    return true;

  std::cerr << "ERROR:ffilesystem:copy_file: " << ec.message() << "\n";
  return false;
#else
  if(overwrite && fs_is_file(dest) && !fs_remove(dest))
    std::cerr << "ERROR:ffilesystem:copy_file: could not remove existing destination file " << dest << "\n";

#if defined(_WIN32)
    if(!CopyFile(source.data(), dest.data(), true)){
      std::cerr << "ERROR:ffilesystem:copy_file: could not copy file " << source << " => " << dest << "\n";
      return false;
    }
#elif defined(__APPLE__) && defined(__MACH__)
  /* copy-on-write file
  * based on kwSys:SystemTools:CloneFileContent
  * https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/copyfile.3.html
  * COPYFILE_CLONE is a 'best try' flag, which falls back to a copy if the clone fails.
  * These flags are meant to be COPYFILE_METADATA | COPYFILE_CLONE, but CLONE
  * forces COPYFILE_NOFOLLOW_SRC and that violates the invariant that this
  * should result in a file.
  * https://gitlab.kitware.com/utils/kwsys/-/commit/ee3223d7ae9a5b52b0a30efb932436def80c0d92
  */
  if(copyfile(source.data(), dest.data(), nullptr,
      COPYFILE_METADATA | COPYFILE_EXCL | COPYFILE_STAT | COPYFILE_XATTR | COPYFILE_DATA) < 0){
    std::cerr << "ERROR:ffilesystem:copy_file: could not clone file " << source << " => " << dest << "\n";
    return false;
  }
#else
    // https://stackoverflow.com/a/29082484
    const int bufferSize = 4096;
    std::string buf(bufferSize, '\0');
    FILE *rid = fopen(source.data(), "r");
    FILE *wid = fopen(dest.data(), "w");

    if (!rid || !wid ) {
      std::cerr << "ERROR:ffilesystem:copy_file: could not open file " << source << " or " << dest << "\n";
      return false;
    }

    while (!feof(rid)) {
      size_t bytes = fread(buf.data(), 1, bufferSize, rid);
      if (bytes)
        fwrite(buf.data(), 1, bytes, wid);
    }

    fclose(rid);
    fclose(wid);
#endif

  return fs_is_file(dest);
#endif
}

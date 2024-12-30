#if defined(__linux__)
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE
#endif
#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64
#endif
#endif

#include "ffilesystem.h"

#include <iostream> // IWYU pragma: keep
#include <system_error>

#include <string>  // IWYU pragma: keep
#include <string_view>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#else
#include <cstdio>  // fopen, fclose, fread, fwrite
#include <cstdlib>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <copyfile.h>
#elif defined(__linux__) || defined(BSD)
#include <sys/types.h>  // off_t
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#endif


bool fs_copy_file(std::string_view source, std::string_view dest, bool overwrite)
{
  // copy a single file -- symlinks are followed

  std::error_code ec;

#ifdef HAVE_CXX_FILESYSTEM
  auto opt = std::filesystem::copy_options::none;
  if (overwrite)
    opt = std::filesystem::copy_options::overwrite_existing;
// WORKAROUND: Windows MinGW GCC 11..13, Intel oneAPI Linux: bug with overwrite_existing failing on overwrite

  if(overwrite && fs_is_file(dest) && !fs_remove(dest)) FFS_UNLIKELY
    fs_print_error(dest, "copy_file:remove");

  if(std::filesystem::copy_file(source, dest, opt, ec) && !ec) FFS_LIKELY
    return true;

#elif defined(_WIN32)
  // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-copyfilea
  if(CopyFileA(source.data(), dest.data(), !overwrite) != 0)
    return true;

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
  auto opt = COPYFILE_METADATA | COPYFILE_STAT | COPYFILE_XATTR | COPYFILE_DATA;
  if (!overwrite)
    opt |= COPYFILE_EXCL;

  const int r = copyfile(source.data(), dest.data(), nullptr, opt);
  if(r == 0)
    return true;
#else

  const int rid = open(source.data(), O_RDONLY);
  if (rid == -1) {
    fs_print_error(source, "copy_file:open");
    return false;
  }

  // leave fstat here to avoid source file race conditino
  struct stat  stat;
  if (fstat(rid, &stat) == -1) {
    fs_print_error(source, "copy_file:fstat");
    close(rid);
    return false;
  }

  off_t len = stat.st_size;

  auto opt = O_CREAT | O_WRONLY | O_TRUNC | O_CLOEXEC;
  if(!overwrite)
    opt |= O_EXCL;

  const int wid = open(dest.data(), opt, 0644);
  if (wid == -1) {
    fs_print_error(dest, "copy_file:open");
    close(rid);
    return false;
  }

  off_t ret = 0;
  int rc = 0;
  int wc = 0;

#if defined(HAVE_COPY_FILE_RANGE)
    // https://man.freebsd.org/cgi/man.cgi?copy_file_range(2)
    // https://man7.org/linux/man-pages/man2/copy_file_range.2.html
    // https://linux.die.net/man/3/open
  if (fs_trace) std::cout << "TRACE::ffilesystem:copy_file: using copy_file_range\n";

  do {
    ret = copy_file_range(rid, nullptr, wid, nullptr, len, 0);
    if (ret == -1)
      break;

    len -= ret;
  } while (len > 0 && ret > 0);

#else

  if (fs_trace) std::cout << "TRACE::ffilesystem:copy_file: using plain file buffer read / write\n";

  const int bufferSize = 16384;
  std::string buf(bufferSize, '\0');

  ssize_t bytes;
  for (len; len > 0; len -= bytes) {
    bytes = read(rid, buf.data(), len);
    if (bytes <= 0 || write(wid, buf.data(), bytes) != bytes) {
      // value should not be zero because we tell the file size in "len"
      close(rid);
      close(wid);
      goto err;
    }
  }

#endif

  rc = close(rid);
  wc = close(wid);

  if(ret >= 0 && rc == 0 && wc == 0)
    return true;

#endif

#if !defined(_MSC_VER) && __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
err:
  fs_print_error(source, "copy_file");
  return false;

}

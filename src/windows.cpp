#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#endif

#include <cstddef>  // for std::byte
#include <iostream>  // IWYU pragma: keep

#include <string>
#include <string_view>

#include "ffilesystem.h"

#if defined(_WIN32)
// create type PREPARSE_DATA_BUFFER
// from ntifs.h, which can only be used by drivers
// typedef is copied from https://gitlab.kitware.com/utils/kwsys/-/blob/master/SystemTools.cxx
// that has a BSD 3-clause license
typedef struct _REPARSE_DATA_BUFFER
{
  ULONG ReparseTag;
  USHORT ReparseDataLength;
  USHORT Reserved;
  union
  {
    struct
    {
      USHORT SubstituteNameOffset;
      USHORT SubstituteNameLength;
      USHORT PrintNameOffset;
      USHORT PrintNameLength;
      ULONG Flags;
      WCHAR PathBuffer[1];
    } SymbolicLinkReparseBuffer;
    struct
    {
      USHORT SubstituteNameOffset;
      USHORT SubstituteNameLength;
      USHORT PrintNameOffset;
      USHORT PrintNameLength;
      WCHAR PathBuffer[1];
    } MountPointReparseBuffer;
    struct
    {
      UCHAR DataBuffer[1];
    } GenericReparseBuffer;
    struct
    {
      ULONG Version;
      WCHAR StringList[1];
      // In version 3, there are 4 NUL-terminated strings:
      // * Package ID
      // * Entry Point
      // * Executable Path
      // * Application Type
    } AppExecLinkReparseBuffer;
  } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
#endif


#if defined(_WIN32)
static bool fs_win32_get_reparse_buffer(std::string_view path, std::byte* buffer)
{

// this function is adapted from
// https://gitlab.kitware.com/utils/kwsys/-/blob/master/SystemTools.cxx
// that has a BSD 3-clause license

  const DWORD attr = GetFileAttributesA(path.data());
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfileattributesa

// https://learn.microsoft.com/en-us/windows/win32/fileio/file-attribute-constants
  if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_REPARSE_POINT))
    return false;

  // Using 0 instead of GENERIC_READ as it allows reading of file attributes
  // even if we do not have permission to read the file itself

  // A reparse point may be an execution alias (Windows Store app), which
  // is similar to a symlink but it cannot be opened as a regular file.
  // We must look at the reparse point data explicitly.

  // FILE_ATTRIBUTE_REPARSE_POINT means:
  // * a file or directory that has an associated reparse point, or
  // * a file that is a symbolic link.
  HANDLE h = CreateFileA(
    path.data(), 0, 0, nullptr, OPEN_EXISTING,
    FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr);

  if (h == INVALID_HANDLE_VALUE)
    return false;

  DWORD bytesReturned = 0;

  BOOL ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, nullptr, 0, buffer,
                        MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &bytesReturned,
                        nullptr);

  CloseHandle(h);

  return ok;
}
#endif


bool fs_is_appexec_alias(
#if __has_cpp_attribute(maybe_unused)
  [[maybe_unused]]
#endif
  std::string_view path)
{
// Windows App Execution Alias allow easy access to Windows Store apps
// from the Windows Terminal. However, they don't work with _access_s()
// or stat() like regular files. This function detects the aliases.
// Reference:
// https://learn.microsoft.com/en-us/windows/terminal/command-line-arguments?tabs=windows#add-windows-terminal-executable-to-your-path
// https://learn.microsoft.com/en-us/windows/win32/fileio/reparse-points
//
// this function is adapted from
// https://gitlab.kitware.com/utils/kwsys/-/blob/master/SystemTools.cxx
// that has a BSD 3-clause license

#if defined(_WIN32)
  std::byte buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];

  if (!fs_win32_get_reparse_buffer(path, buffer))
    return false;

  PREPARSE_DATA_BUFFER data =
    reinterpret_cast<PREPARSE_DATA_BUFFER>(&buffer[0]);

  // https://learn.microsoft.com/en-us/windows/win32/fileio/reparse-point-tags
  return data->ReparseTag == IO_REPARSE_TAG_APPEXECLINK;

#else
  return false;
#endif

}


bool fs_win32_is_symlink(
#if __has_cpp_attribute(maybe_unused)
  [[maybe_unused]]
#endif
  std::string_view path)
{
// distinguish between Windows symbolic links and reparse points as
// reparse points can be unlike symlinks.
//
// this function is adapted from
// https://gitlab.kitware.com/utils/kwsys/-/blob/master/SystemTools.cxx
// that has a BSD 3-clause license

#if defined(_WIN32)
  std::byte buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
  // Since FILE_ATTRIBUTE_REPARSE_POINT is set this file must be
  // a symbolic link if it is not a reparse point.
  if (!fs_win32_get_reparse_buffer(path, buffer))
    return GetLastError() == ERROR_NOT_A_REPARSE_POINT;

  ULONG reparseTag =
    reinterpret_cast<PREPARSE_DATA_BUFFER>(&buffer[0])->ReparseTag;

  return (reparseTag == IO_REPARSE_TAG_SYMLINK) ||
         (reparseTag == IO_REPARSE_TAG_MOUNT_POINT);
#else
  return false;
#endif
}


std::string fs_win32_full_name(std::string_view path)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnameA
// GetFinalPathNameByHandle or GetFullPathName returns the unresolved symlink

#ifdef _WIN32
  const DWORD L = GetFullPathNameA(path.data(), 0, nullptr, nullptr);
  if(L == 0)  FFS_UNLIKELY
  {
    fs_print_error(path, "realpath:GetFullPathName");
    return {};
  }
  // this form includes the null terminator
  std::string r(L, '\0');
  // weak detection of race condition (cwd change)
  if(GetFullPathNameA(path.data(), L, r.data(), nullptr) == L-1)  FFS_LIKELY
  {
    r.resize(L-1);
    return fs_as_posix(r);
  }

  fs_print_error(path, "realpath:GetFullPathName");
  return {};
#else
  return std::string(path);
#endif
}


std::string fs_win32_final_path(std::string_view path)
{
  // resolves Windows symbolic links (reparse points and junctions)
  // it also resolves the case insensitivity of Windows paths to the disk case
  // PATH MUST EXIST
  //
  // References:
  // https://stackoverflow.com/a/50182947
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
  // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlea

#if defined(_WIN32)
  if(fs_trace) std::cout << "TRACE: win32_final_path(" << path << ")\n";
  // dwDesiredAccess=0 to allow getting parameters even without read permission
  // FILE_FLAG_BACKUP_SEMANTICS required to open a directory
  HANDLE h = CreateFileA(path.data(), GENERIC_READ, FILE_SHARE_READ, nullptr,
              OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
  if(h == INVALID_HANDLE_VALUE)
    return {};

  std::string r(fs_get_max_path(), '\0');

  const DWORD L = GetFinalPathNameByHandleA(h, r.data(), static_cast<DWORD>(r.size()), FILE_NAME_NORMALIZED);
  CloseHandle(h);
  if(L == 0){
    fs_print_error("win32_final_path:GetFinalPathNameByHandle", path);
    return {};
  }

  r.resize(L);

#ifdef __cpp_lib_starts_ends_with  // C++20
  if (r.starts_with("\\\\?\\"))
#else  // C++98
  if (r.substr(0, 4) == "\\\\?\\")
#endif
    r = r.substr(4);

  return fs_as_posix(r);
#else
  return std::string(path);
#endif
}


std::string fs_longname(std::string_view in)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea
// the path must exist
#ifdef _WIN32
  std::string out(fs_get_max_path(), '\0');
// size does not include null terminator

  if(auto L = GetLongPathNameA(in.data(), out.data(), static_cast<DWORD>(out.size()));
      L > 0 && L < out.length()){
    out.resize(L);
    return out;
  }

  fs_print_error(in, "longname");
  return {};
#else
  return std::string(in);
#endif
}


std::string fs_shortname(std::string_view in)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getshortpathnamew
// the path must exist
#ifdef _WIN32
  std::string out(fs_get_max_path(), '\0');
// size does not include null terminator
  if(auto L = GetShortPathNameA(in.data(), out.data(), static_cast<DWORD>(out.size()));
      L > 0 && L < out.length()){
    out.resize(L);
    return out;
  }

  fs_print_error(in, "shortname");
  return {};
#else
  return std::string(in);
#endif
}


std::string fs_win32_to_narrow(
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
  std::wstring_view w)
{

#if defined(_WIN32)
  int L = WideCharToMultiByte(CP_UTF8, 0, w.data(), -1, nullptr, 0, nullptr, nullptr);
  if (L > 0) {
    std::string buf(L, '\0');
    L = WideCharToMultiByte(CP_UTF8, 0, w.data(), -1, buf.data(), L, nullptr, nullptr);

    if(L > 0){
      buf.resize(L-1);
      return buf;
    }
  }
#endif

  fs_print_error("", "fs_win32_to_narrow");
  return {};
}


std::wstring fs_win32_to_wide(
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
  std::string_view n)
{
#if defined(_WIN32)
  // https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
  int L = MultiByteToWideChar(CP_UTF8, 0, n.data(), -1, nullptr, 0);
  if (L > 0) {
    std::wstring buf(L, L'\0');
    L = MultiByteToWideChar(CP_UTF8, 0, n.data(), -1, buf.data(), L);

    if(L > 0){
      buf.resize(L-1);
      return buf;
    }
  }
#endif

  fs_print_error("", "fs_win32_to_wide");
  return {};
}

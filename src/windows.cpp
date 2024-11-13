#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>  // IWYU pragma: keep

#include <string>
#include <string_view>

#include <algorithm> // std::replace

#include "ffilesystem.h"


std::string fs_as_posix(std::string_view path)
{
  std::string s(path);

  if(fs_is_windows())
    std::replace( s.begin(), s.end(), '\\', '/');

  return s;
}


std::string fs_win32_full_name(std::string_view path)
{
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnameA
// GetFinalPathNameByHandle or GetFullPathName returns the unresolved symlink

#ifdef _WIN32
  const DWORD L = GetFullPathNameA(path.data(), 0, nullptr, nullptr);
  if(L == 0){  FFS_UNLIKELY
    fs_print_error(path, "realpath:GetFullPathName");
    return {};
  }
  // this form includes the null terminator
  std::string r(L, '\0');
  // weak detection of race condition (cwd change)
  if(GetFullPathNameA(path.data(), L, r.data(), nullptr) == L-1){  FFS_LIKELY
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
  if(FS_TRACE) std::cout << "TRACE: win32_final_path(" << path << ")\n";
  // dwDesiredAccess=0 to allow getting parameters even without read permission
  // FILE_FLAG_BACKUP_SEMANTICS required to open a directory
  HANDLE h = CreateFileA(path.data(), 0, 0, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
  if(h == INVALID_HANDLE_VALUE){
    fs_print_error(path, "win32_final_path:CreateFile");
    return {};
  }

  std::string r(fs_get_max_path(), '\0');

  DWORD L = GetFinalPathNameByHandleA(h, r.data(), r.size(), FILE_NAME_NORMALIZED);
  CloseHandle(h);

  switch (L) {
    case ERROR_PATH_NOT_FOUND:
      std::cerr << "ERROR:win32_final_path:GetFinalPathNameByHandle: path not found\n";
      return {};
    case ERROR_NOT_ENOUGH_MEMORY:
      std::cerr << "ERROR:win32_final_path:GetFinalPathNameByHandle: buffer too small\n";
      return {};
    case ERROR_INVALID_PARAMETER:
      std::cerr << "ERROR:win32_final_path:GetFinalPathNameByHandle: invalid parameter\n";
      return {};
    case 0:
      std::cerr << "ERROR:win32_final_path:GetFinalPathNameByHandle: unknown error\n";
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

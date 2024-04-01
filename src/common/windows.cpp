#include "ffilesystem.h"

#include <memory>

#if defined(_WIN32) || defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <string>
#include <cstddef> // size_t
#include <iostream>

#include <filesystem>


size_t fs_shortname(const char* in, char* out, size_t buffer_size){
  return fs_str2char(Ffs::shortname(std::string_view(in)), out, buffer_size);
}

std::string Ffs::shortname(std::string_view in){
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getshortpathnamew
// the path must exist

  std::filesystem::path p(in);

#ifdef _WIN32
  auto buf = std::make_unique<char[]>(fs_get_max_path());
// size includes null terminator
  DWORD L = GetShortPathNameA(in.data(), nullptr, 0);
  if (L == 0) FFS_UNLIKELY
  {
    std::cerr << "ERROR:Ffs::shortname:GetShortPathName: could not determine short path length " << p << "\n";
    return {};
  }

// convert long path
  if(!GetShortPathNameA(in.data(), buf.get(), L)) FFS_UNLIKELY
  {
    std::cerr << "ERROR:Ffs::shortname:GetShortPathName: could not determine short path " << p << "\n";
    return {};
  }

  std::string out(buf.get());
#else
  std::cerr << "WARNING:Ffs::shortname:ffilesystem: Windows-only\n";
  std::string out(in);
#endif
  return out;
}


size_t fs_longname(const char* in, char* out, size_t buffer_size){
  return fs_str2char(Ffs::longname(std::string_view(in)), out, buffer_size);
}

std::string Ffs::longname(std::string_view in){
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea
// the path must exist

  std::filesystem::path p(in);

#ifdef _WIN32
    auto buf = std::make_unique<char[]>(fs_get_max_path());
// size includes null terminator
    DWORD L = GetLongPathNameA(in.data(), nullptr, 0);
    if(L == 0) FFS_UNLIKELY
    {
    std::cerr << "ERROR:Ffs::longname:GetLongPathName: could not determine path length " << p << "\n";
    return {};
  }

// convert short path
    if(!GetLongPathNameA(in.data(), buf.get(), L)) FFS_UNLIKELY
    {
    std::cerr << "ERROR:Ffs::longname:GetLongPathName: could not determine path " << p << "\n";
    return {};
  }
    std::string out(buf.get());
#else
    std::cerr << "WARNING:ffilesystem:Ffs::longname: Windows-only\n";
    std::string out(in);
#endif

    return out;
}

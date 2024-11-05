#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <aclapi.h> // GetNamedSecurityInfo
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>      // for getpwuid
#include <grp.h>     // for getgrgid
#endif

#if defined(__linux__) && defined(USE_STATX)
#include <fcntl.h>   // AT_* constants for statx()
#endif

#include <string>
#include <iostream>

#include "ffilesystem.h"

std::string fs_get_owner_name(std::string_view path)
{
#ifdef _WIN32
// https://learn.microsoft.com/en-us/windows/win32/secauthz/finding-the-owner-of-a-file-object-in-c--

  DWORD Lowner = 0;
  DWORD Ldomain = 0;
  PSECURITY_DESCRIPTOR pSD = nullptr;
  PSID pOwnerSid = nullptr;
  SID_NAME_USE eUse = SidTypeUnknown;

  // https://learn.microsoft.com/en-us/windows/win32/api/aclapi/nf-aclapi-getnamedsecurityinfoa
  DWORD dwResult = GetNamedSecurityInfoA(path.data(), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pOwnerSid, nullptr, nullptr, nullptr, &pSD);
  if (dwResult != ERROR_SUCCESS) {
    fs_print_error(path, "owner:GetNamedSecurityInfo: failed to get security info");
    return {};
  }

// get buffer size

  // segfaults if you do not use dwDomainName and eUse
  if (!LookupAccountSidA(nullptr, pOwnerSid, nullptr, &Lowner,
                         nullptr, &Ldomain, &eUse))
  {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
      fs_print_error(path, "owner:LookupAccountSid: get buffer size failed");
      return {};
    }
  }

  std::string owner(Lowner, '\0');
  const auto r = LookupAccountSidA(nullptr, pOwnerSid, owner.data(), &Lowner, nullptr, &Ldomain, &eUse);
  LocalFree(pSD);
  if (!r) {
    fs_print_error(path, "owner:LookupAccountSid: get owner name failed");
    return {};
  }
  owner.resize(Lowner);
  return owner;

#else

#if defined(STATX_UID) && defined(USE_STATX)
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (FS_TRACE) std::cout << "TRACE: statx() owner_name " << path << "\n";
  struct statx s;
  if(statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_UID, &s) == 0){
    auto pw = getpwuid(s.stx_uid);
#else
  struct stat s;
  if(!stat(path.data(), &s)){
    auto pw = getpwuid(s.st_uid);
#endif
    return pw->pw_name;
  }

  fs_print_error(path, "get_owner:getpwuid");
  return {};
#endif
}


std::string fs_get_owner_group(std::string_view path)
{
#ifdef _WIN32
  // use GetNamedSecurityInfoA to get group name

  DWORD Lgroup = 0;
  DWORD Ldomain = 0;
  PSECURITY_DESCRIPTOR pSD = nullptr;
  PSID pGroupSid = nullptr;
  SID_NAME_USE eUse = SidTypeUnknown;

  // https://learn.microsoft.com/en-us/windows/win32/api/aclapi/nf-aclapi-getnamedsecurityinfoa
  DWORD dwResult = GetNamedSecurityInfoA(path.data(), SE_FILE_OBJECT, GROUP_SECURITY_INFORMATION, nullptr, &pGroupSid, nullptr, nullptr, &pSD);
  if (dwResult != ERROR_SUCCESS) {
    fs_print_error(path, "get_owner_group:GetNamedSecurityInfo: failed to get security info");
    return {};
  }

  // get buffer size
  if (!LookupAccountSidA(nullptr, pGroupSid, nullptr, &Lgroup,
                         nullptr, &Ldomain, &eUse))
  {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
      fs_print_error(path, "get_owner_group:LookupAccountSid: get buffer size failed");
      return {};
    }
  }


  std::string group(Lgroup, '\0');
  const auto r = LookupAccountSidA(nullptr, pGroupSid, group.data(), &Lgroup, nullptr, &Ldomain, &eUse);
  LocalFree(pSD);
  if (!r) {
    fs_print_error(path, "get_owner_group:LookupAccountSid: get group name failed");
    return {};
  }
  group.resize(Lgroup);
  return group;

#else

#if defined(STATX_GID) && defined(USE_STATX)
// https://www.man7.org/linux/man-pages/man2/statx.2.html
  if (FS_TRACE) std::cout << "TRACE: statx() owner_name " << path << "\n";
  struct statx s;
  if(statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_GID, &s) == 0){
    auto gr = getgrgid(s.stx_gid);
#else
  struct stat s;
  if(!stat(path.data(), &s)){
    auto gr = getgrgid(s.st_gid);
#endif
    return gr->gr_name;
  }

  fs_print_error(path, "get_owner_group");
  return {};

#endif
}

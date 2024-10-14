#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <aclapi.h> // GetNamedSecurityInfo
#include <Windows.h>
#else
#include <sys/types.h>
#include <pwd.h>      // for getpwuid
// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>     // for getgrgid
#endif

#include <string>

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

  const LPSTR OwnerName = (LPTSTR)GlobalAlloc(GMEM_FIXED, Lowner*sizeof(TCHAR));
  if (!OwnerName) {
    fs_print_error(path, "owner:GlobalAlloc: failed to allocate memory");
    return {};
  }

  if (!LookupAccountSidA(nullptr, pOwnerSid, OwnerName, &Lowner, nullptr, &Ldomain, &eUse)) {
    fs_print_error(path, "owner:LookupAccountSid: get owner name failed");
    GlobalFree(OwnerName);
    return {};
  }

  std::string owner(OwnerName);
  LocalFree(pSD);
  GlobalFree(OwnerName);
  return owner;

#else

  struct stat s;
  if(stat(path.data(), &s)){
     fs_print_error(path, "get_owner:stat");
    return {};
  }

  const struct passwd *pw = getpwuid(s.st_uid);
  if(!pw){
    fs_print_error(path, "get_owner:getpwuid");
    return {};
  }

  return pw->pw_name;

#endif
}


std::string fs_get_owner_group(std::string_view path)
{
#ifdef _WIN32
  fs_print_error(path, "get_owner_group: not implemented for Windows");
  return {};
#else
  struct stat s;
  if(stat(path.data(), &s)){
     fs_print_error(path, "get_owner_group:stat");
    return {};
  }

  const struct group *grp = getgrgid(s.st_gid);
  if(!grp){
    fs_print_error(path, "get_owner_group:getgrgid");
    return {};
  }

  return grp->gr_name;
#endif
}

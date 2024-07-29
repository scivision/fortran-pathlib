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
#include <errno.h>
#endif



#include <string.h>
#include <stdio.h>

#include "ffilesystem.h"

size_t fs_get_owner(const char* path, char* name, const size_t buffer_size)
{
#ifdef _WIN32
// https://learn.microsoft.com/en-us/windows/win32/secauthz/finding-the-owner-of-a-file-object-in-c--

  DWORD Lowner = 0;
  DWORD Ldomain = 0;
  PSECURITY_DESCRIPTOR pSD = NULL;
  PSID pOwnerSid = NULL;
  LPTSTR OwnerName = NULL;
  SID_NAME_USE eUse = SidTypeUnknown;

  DWORD dwResult = GetNamedSecurityInfoA(path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pOwnerSid, NULL, NULL, NULL, &pSD);
  if (dwResult != ERROR_SUCCESS) {
    fs_win32_print_error(path, "owner:GetNamedSecurityInfo: failed to get security info");
    return 0;
  }

// get buffer size

  // segfaults if you do not use dwDomainName and eUse
  if (!LookupAccountSidA(NULL, pOwnerSid, NULL, &Lowner,
                         NULL, &Ldomain, &eUse))
  {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
      fs_win32_print_error(path, "owner:LookupAccountSid: get buffer size failed");
      return 0;
    }
  }

  OwnerName = (LPTSTR)GlobalAlloc(GMEM_FIXED, Lowner*sizeof(TCHAR));
  if (OwnerName == NULL) {
    fs_win32_print_error(path, "owner:GlobalAlloc: failed to allocate memory");
    return 0;
  }

  if (!LookupAccountSidA(NULL, pOwnerSid, OwnerName, &Lowner, NULL, &Ldomain, &eUse)) {
    fs_win32_print_error(path, "owner:LookupAccountSid: get owner name failed");
    GlobalFree(OwnerName);
    return 0;
  }

  size_t L = fs_strncpy(OwnerName, name, buffer_size);
  LocalFree(pSD);
  GlobalFree(OwnerName);
  return L;

#else
  struct stat s;
  if(stat(path, &s)){
     fprintf(stderr, "ERROR:ffilesystem:fs_get_owner:stat: %s => %s\n", path, strerror(errno));
    return 0;
  }

  struct passwd *pw = getpwuid(s.st_uid);
  if(pw == NULL){
    fprintf(stderr, "ERROR:ffilesystem:fs_get_owner:getpwuid %s => %s\n", path, strerror(errno));
    return 0;
  }

  return fs_strncpy(pw->pw_name, name, buffer_size);
#endif
}

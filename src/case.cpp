#include "ffilesystem.h"

#include <string_view>
#include <string>

#include <algorithm> // std::transform
#include <cctype> // std::tolower

bool fs_is_case_sensitive(std::string_view path)
{
  // check if the filesystem is case sensitive
  if(path.empty())
    return false;

  const std::string rname = fs_generate_random_alphanumeric_string(16);
  // create a temporary file with a unique name that has CamelCase
  const std::string CamelCase = fs_join(path, ".FfilesystemCaseSensitiveCheck_" + rname);

  if(!fs_touch(CamelCase))
    return false;

  std::string lower = CamelCase;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  const std::string actual = fs_canonical(lower, true, true);

  if(!rname.empty())
    fs_remove(CamelCase);

  return actual == CamelCase;

}
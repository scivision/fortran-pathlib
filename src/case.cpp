#include "ffilesystem.h"

#include <string_view>
#include <string>

#include <algorithm> // tolower

bool fs_is_case_sensitive(std::string_view path)
{
  // check if the filesystem is case sensitive
  if(path.empty())
    return false;

  // create a temporary file with a unique name that has CamelCase
  const std::string CamelCase = fs_join(path, ".FfilesystemCaseSensitiveCheck_" + fs_generate_random_alphanumeric_string(12));

  if(!fs_touch(CamelCase))
    return false;

  std::string lower = CamelCase;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  const std::string actual = fs_canonical(lower, true, true);

  fs_remove(CamelCase);

  return actual == CamelCase;

}

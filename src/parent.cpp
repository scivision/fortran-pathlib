#include "ffilesystem.h"

#include <string>
#include <string_view>

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#include <iostream> // IWYU pragma: keep
#else
#include <vector>
#endif



std::string fs_parent(std::string_view path)
{

  if(path.empty())
    return ".";

  std::string p = fs_drop_slash(path);
#ifdef HAVE_CXX_FILESYSTEM
  // have to drop_slash on input to get expected parent path -- necessary for AppleClang
  p = std::filesystem::path(p).parent_path().generic_string();

  if(FS_TRACE) std::cout << "TRACE:parent(" << path << ") => " << p << "\n";

// 30.10.7.1 [fs.path.generic] dot-dot in the root-directory refers to the root-directory itself.
// On Windows, a drive specifier such as "C:" or "z:" is treated as a root-name.
// On Cygwin, a path that begins with two successive directory separators is a root-name.
// Otherwise (for POSIX-like systems other than Cygwin), the implementation-defined root-name
// is an unspecified string which does not appear in any pathnames.

#else

  // don't fully normalize to preserve possible symlinks above parent
  std::vector<std::string> parts = fs_split(p);
  p.clear();

  if(parts.empty())
    return ".";

  if (fs_is_windows() && parts.size() == 1 && !fs_root_name(parts[0]).empty())
    return fs_root(path);

  // drop last part
  parts.pop_back();

  // rebuild path
  // preserve leading slash
  if (path.front() == '/')
    p.push_back('/');

  for (const auto& part : parts){
    if(!part.empty())
      p.append(part).push_back('/');
  }

  if(p.length() > 1 && p.back() == '/')
    p.pop_back();

#endif

  // handle "/" and other no parent cases
  if (p.empty()){
    if (!path.empty() && path.front() == '/')
      return "/";
    else
      return ".";
  }

  // need this for <filesystem> and our method to make x: x:/
  if (fs_is_windows() && p.length() == 2 && !fs_root_name(p).empty())
    p.push_back('/');

  return p;
}

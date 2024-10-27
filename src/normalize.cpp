#include "ffilesystem.h"

#include <string>
#include <vector>


std::string fs_normal(std::string_view path)
{
  // normalize path
   std::string r;
#ifdef HAVE_CXX_FILESYSTEM
  r = std::filesystem::path(path).lexically_normal().generic_string();
#else
  const std::vector<std::string> parts = fs_split(path);

  std::vector<std::string> new_parts;

  // drop repeated slashes, "." and ".."
  for (const auto& part : parts) {
    if (part.empty())
      continue;

    if (part == ".")
      continue;

    if (part == "..") {

      if (new_parts.empty()){
        if(path[0] != '/')
            new_parts.push_back(part);
      } else {
        if (new_parts.back() == "..")
          new_parts.push_back(part);
        else
          new_parts.pop_back();
      }

      continue;
    }

    new_parts.push_back(part);
  }

  // rebuild path
   if (path[0] == '/' || (fs_is_windows() && path[0] == '\\'))
     r = "/";

  for (const auto& part : new_parts)
    r += part + "/";

#endif

  // no trailing slash
  if(r.length() > 1 && r.back() == '/'){
    if(!fs_is_windows() || r != fs_root(r))
      r.pop_back();
  }

  if(r.empty())
    r = ".";

  return r;
}


std::string fs_drop_slash(std::string_view sv)
{
  // drop all trailing "/"
  std::string s(sv);
  while(s.length() > 1 && (s.back() == '/' || (fs_is_windows() && s.back() == '\\')))
    s.pop_back();
  return s;
}


std::string fs_trim(std::string r)
{
  // trim trailing nulls
  if(const auto i = r.find('\0');
      i != std::string::npos)
    r.resize(i);

  return r;
}

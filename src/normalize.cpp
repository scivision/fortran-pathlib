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

    if (part == "."){
      if (new_parts.empty())
        new_parts.push_back(part);

      continue;
    }

    if (part == "..") {
      if (new_parts.empty() || new_parts.back() == "..")
        new_parts.push_back(part);
      else
        new_parts.pop_back();

      continue;
    }

    if(new_parts.size() == 1 && new_parts[0] == ".")
      new_parts.pop_back();

    new_parts.push_back(part);
  }

  // rebuild path, without trailing slash
  // preserve leading slash
  if (path[0] == '/')
    r = "/";

  for (const auto& part : new_parts){
    r += part + "/";
  }

#endif

  if(r.length() > 1 && r.back() == '/')
    r.pop_back();

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

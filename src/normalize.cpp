#include "ffilesystem.h"

#include <string>
#include <vector>
#include <iostream>


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


std::vector<std::string> fs_split(std::string_view path)
{
  if(path.empty())
    return {};
  // break paths into components
  std::vector<std::string> parts;
  std::string p = fs_as_posix(path);

  // no empty trailing part for trailing slash
  if (p.back() == '/')
    p.pop_back();

  // split path, including last component
  size_t start = 0;
  size_t end;

  do {
    end = p.find_first_of('/', start);
    if(FS_TRACE) std::cout << "TRACE: split " << start << " " << end << " " << path << " " << p.substr(start, end-start) << "\n";
    parts.push_back(p.substr(start, end - start));
    start = end + 1;
  } while (end != std::string::npos);

  return parts;
}

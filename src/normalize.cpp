#include "ffilesystem.h"

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#endif


#include <algorithm> // std::unique
#include <string>
#include <string_view>

#include <vector>
#include <iostream>


std::vector<std::string>
fs_normal_vector(std::string_view path)
{
  if(path.empty())
    return {};

  const std::vector<std::string> parts = fs_split(path);

  std::vector<std::string> n;

  // drop repeated slashes, "." and ".."
  for (const auto& p : parts) {
    if (p.empty() || p == ".")
      continue;

    if (p == "..") {
      if (!n.empty() && n.back() != "..")
        n.pop_back();
      else if (path.front() != '/')
        n.push_back(p);
      continue;
    }

    n.push_back(p);
  }

  return n;
}


std::string
fs_normal(std::string_view path)
{
  // normalize path
   std::string r;
#ifdef HAVE_CXX_FILESYSTEM
  r = std::filesystem::path(path).lexically_normal().generic_string();
#else
  // leave the empty short-circuit to avoid needless computation
  // and avoid indexing into an empty string
  if (path.empty())
    return ".";

  const std::vector<std::string> parts = fs_normal_vector(path);

  // rebuild path
   if (fs_slash_first(path))
     r.push_back('/');

  for (const auto& p : parts)
    r.append(p).push_back('/');

#endif

  // no trailing slash
  if (r.length() > 1 && r.back() == '/' && (!fs_is_windows() || r != fs_root(r)))
    r.pop_back();

  if (r.empty())
    r.push_back('.');

  return r;
}


std::string
fs_drop_slash(std::string_view sv)
{
  // drop all trailing "/" and duplicated internal "/"
  std::string s(sv);
  while(s.length() > 1 && (s.back() == '/' || (fs_is_windows() && s.back() == '\\')))
    s.pop_back();

  s.erase(std::unique(s.begin(), s.end(), [](char a, char b){ return a == '/' && b == '/'; }), s.end());

  return s;
}


std::string
fs_trim(std::string r)
{
  // trim trailing nulls
  if(const auto i = r.find('\0');
      i != std::string::npos)
    r.resize(i);

  return r;
}


std::vector<std::string>
fs_split(std::string_view path)
{
  if(path.empty())
    return {};

  // break paths into non-empty components
  std::vector<std::string> parts;
  std::string p = fs_as_posix(path);

  // split path, including last component
  std::string::size_type start = 0;
  std::string::size_type end;

  while (true) {
    end = p.find('/', start);
    if(fs_trace) std::cout << "TRACE: split " << start << " " << end << " " << path << " " << p.substr(start, end-start) << "\n";

    // last component
    if (end == std::string::npos){
      parts.push_back(p.substr(start));
      break;
    }

    // do not add empty parts
    if (end != start)
      parts.push_back(p.substr(start, end - start));

    start = end + 1;
  }

  return parts;
}

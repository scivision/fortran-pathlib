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
fs_drop_slash(std::string_view path)
{
  // drop all trailing "/" and duplicated internal "/"
  const std::string p = fs_as_posix(path);
  if(p.empty())
    return {};

  std::string s(p);

  s.erase(std::unique(s.begin(), s.end(), [](char a, char b){ return a == '/' && b == '/'; }), s.end());

  if(s.empty() || s == "/")
    return s;

  // don't return invalid path c:, but rather c:/
  if(s.back() == '/' && (!fs_is_windows() || (s.length() != 3 || s != fs_root(p))))
    s.pop_back();

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

  do {
    end = p.find('/', start);
    if(fs_trace) std::cout << "TRACE: split " << start << " " << end << " " << path << " " << p.substr(start, end-start) << "\n";
    // do not add empty parts
    if (end != start)
      parts.push_back(p.substr(start, end - start));
    start = end + 1;
  } while (end != std::string::npos);

  return parts;
}

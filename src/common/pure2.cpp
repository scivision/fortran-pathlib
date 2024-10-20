// transition file for pure.c

#include <string>
#include <cctype>
#include <vector>
#include <iostream>

#include "ffilesystem.h"


// tell if Ffilesystme core is C or C++
bool fs_cpp(){ return true; }

// C++ version compiler claims to support with given options
long fs_lang(){ return __cplusplus; }


bool fs_is_absolute(std::string_view path)
{
  if(path.empty())
    return false;

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).is_absolute();
#else
  if(fs_is_windows())
    return path.length() > 2 && std::isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || (fs_is_windows() && path[2] == '\\'));
  else
    return path[0] == '/';
#endif
}


std::string fs_file_name(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().generic_string();
#else
  std::string r = fs_as_posix(path);

  if (auto pos = r.find_last_of('/');
        pos != std::string::npos)
    return std::string(r.substr(pos + 1));

  return r;
#endif
}


std::string fs_root(std::string_view path)
{
  // our root = root_name / root_directory
  // empty path is not absolute
  if(!fs_is_absolute(path))
    return {};

#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).root_path().generic_string();
#else
  return fs_is_windows() ? std::string(1, path[0]) + ":/" : "/";
#endif
}


std::string fs_stem(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().stem().generic_string();
#else
  std::string r = fs_file_name(path);
  if(r.empty())
    return {};

  // handle special case a/..
  if (r == "..")
    return r;

  // find last dot
  if (auto pos = r.find_last_of('.');
        pos != std::string::npos && pos != 0)
    return std::string(r.substr(0, pos));

  return r;
#endif
}


std::string fs_parent(std::string_view path)
{
  std::string p;
#ifdef HAVE_CXX_FILESYSTEM
  // have to drop_slash on input to get expected parent path -- necessary for AppleClang
  p = std::filesystem::path(fs_drop_slash(path)).parent_path().generic_string();

  // remove repeated path seperators from p string
  p.erase(std::unique(p.begin(), p.end(), [](char a, char b){ return a == '/' && b == '/'; }), p.end());

  if(FS_TRACE) std::cout << "TRACE:parent(" << path << ") => " << p << "\n";

// 30.10.7.1 [fs.path.generic] dot-dot in the root-directory refers to the root-directory itself.
// On Windows, a drive specifier such as "C:" or "z:" is treated as a root-name.
// On Cygwin, a path that begins with two successive directory separators is a root-name.
// Otherwise (for POSIX-like systems other than Cygwin), the implementation-defined root-name
// is an unspecified string which does not appear in any pathnames.

#else

  std::vector<std::string> parts = fs_split(path);

  // drop empty last parts
  while(parts.size() > 1 && parts.back().empty())
    parts.pop_back();

  if(parts.empty())
    return ".";

  if (fs_is_windows() && parts.size() == 1 && parts[0].length() == 2 && std::isalpha(parts[0][0]) && parts[0][1] == ':')
    return parts[0] + "/";

  // drop last part
  parts.pop_back();

  // rebuild path
  // preserve leading slash
  if (path[0] == '/')
    p = "/";

  for (const auto& part : parts){
    if(!part.empty())
      p += part + "/";
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

  if (fs_is_windows() && p.length() == 2 && std::isalpha(p[0]) && p[1] == ':')
    p += "/";

  return p;
}


std::string fs_suffix(std::string_view path)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).filename().extension().generic_string();
#else
  const std::string p = fs_file_name(path);
  if(p.empty())
    return {};

  // find last dot
  if (auto pos = p.find_last_of('.');
        pos != std::string::npos && pos != 0)
    return std::string(p.substr(pos));

  return {};
#endif
}


std::string fs_join(std::string_view path, std::string_view other){
#ifdef HAVE_CXX_FILESYSTEM
  return fs_drop_slash((std::filesystem::path(path) / other).lexically_normal().generic_string());
#else
  if(path.empty() && other.empty())
    return {};

  if(path.empty())
    return fs_drop_slash(other);

  if(other.empty())
    return fs_drop_slash(path);

  if (other[0] == '/' || (fs_is_windows() && fs_is_absolute(other)))
    return fs_drop_slash(other);

  return fs_normal(std::string(path) + "/" + std::string(other));
#endif
}



std::vector<std::string> fs_split(std::string_view path)
{
  if(path.empty())
    return {};
  // break paths into components
  std::vector<std::string> parts;
  std::string p = fs_as_posix(path);
  // split path, including last component
  size_t start = 0;
  size_t end;
  do {
    end = p.find_first_of('/', start);
    parts.push_back(p.substr(start, end - start));
    start = end + 1;
  } while (end != std::string::npos);
  return parts;
}


std::string fs_normal(std::string_view path)
{
  // normalize path
#ifdef HAVE_CXX_FILESYSTEM
  return fs_drop_slash(std::filesystem::path(path).lexically_normal().generic_string());
#else
  const std::vector<std::string> parts = fs_split(path);
  if(parts.empty())
    return {};
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
  std::string r;
  // preserve leading slash
  if (path[0] == '/')
    r = "/";

  for (const auto& part : new_parts){
    r += part + "/";
  }

  if(r.length() > 1 && r.back() == '/')
    r.pop_back();

  return r;
#endif
}


std::string fs_drop_slash(std::string_view sv)
{
  // drop all trailing "/"
  std::string s(sv);
  while(s.length() > 1 && (s.back() == '/' || (fs_is_windows() && s.back() == '\\')))
    s.pop_back();
  return s;
}


std::string fs_relative_to(std::string_view base, std::string_view other)
{
  // find lexical relative path from base to other
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(other).lexically_relative(base).lexically_normal().generic_string();
#else

  if(base.empty() && other.empty())
    return ".";

  if(base.empty() || other.empty())
    return {};

  const std::string b = fs_normal(base);
  const std::string o = fs_normal(other);

  if(b == o)
    return ".";

  const std::vector<std::string> parts = fs_split(b);
  const std::vector<std::string> other_parts = fs_split(o);

  const std::string::size_type Lb = parts.size();
  const std::string::size_type Lo = other_parts.size();

  // find common prefix, returning empty if no common prefix
  size_t i = 0;
  for (; i < Lb && i < Lo; i++)
    if (parts[i] != other_parts[i])
      break;

  if (i == 0 && parts[0] != other_parts[0])
    return {};


  // build relative path

  std::string r;
  for (size_t j = i; j < parts.size(); j++)
    r += "../";

  for (size_t j = i; j < other_parts.size(); j++)
    r += other_parts[j] + "/";

  return fs_drop_slash(r);

#endif
}



std::string fs_proximate_to(std::string_view base, std::string_view other)
{
// proximate_to is LEXICAL operation
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(other).lexically_proximate(base).lexically_normal().generic_string();
#else
  const std::string r = fs_relative_to(base, other);
  return (r.empty()) ? std::string(other) : r;
#endif
}


bool fs_is_subdir(std::string_view subdir, std::string_view dir)
{
  // is subdir a subdirectory of dir

  const std::string r = fs_relative_to(dir, subdir);

  return !r.empty() && r != "." &&
#ifdef __cpp_lib_starts_ends_with
    !r.starts_with("..");
#else
    r.substr(0,2) != "..";
#endif

}


std::string fs_with_suffix(std::string_view path, std::string_view new_suffix)
{
#ifdef HAVE_CXX_FILESYSTEM
  return std::filesystem::path(path).replace_extension(new_suffix).generic_string();
#else

  const std::string parent = fs_parent(path);
  const std::string stem = fs_stem(path);

  const std::string r = (parent == ".") ? stem : fs_join(parent, stem);

  return r + std::string(new_suffix);

#endif
}

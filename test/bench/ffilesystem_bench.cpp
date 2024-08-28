#include <chrono>
#include <string>
#include <algorithm>
#include <iostream>
#include <functional>
#include <map>
#include <set>
#include <cstdlib>
#include <vector>

#include "ffilesystem.h"
#include "ffilesystem_bench.h"


std::chrono::duration<double> bench_c(int n, std::string_view path, std::string_view fname, bool verbose)
{

std::map<std::string_view, std::function<size_t(char*, size_t)>> s_ =
  {
    {"compiler", fs_compiler},
    {"homedir", fs_get_homedir},
    {"cwd", fs_get_cwd},
    {"tempdir", fs_get_tempdir}
  };

std::map<std::string_view, std::function<size_t(const char*, char*, size_t)>> s_s =
  {
    {"expanduser", fs_expanduser},
    {"which", fs_which},
    {"parent", fs_parent},
    {"root", fs_root},
    {"stem", fs_stem},
    {"suffix", fs_suffix},
    {"filename", fs_file_name},
    {"perm", fs_get_permissions},
    {"read_symlink", fs_read_symlink},
    {"normal", fs_normal},
    {"getenv", fs_getenv}
  };

std::map<std::string_view, std::function<size_t(const char*, bool, char*, size_t)>> ssb =
  {
    {"canonical", fs_canonical},
    {"resolve", fs_resolve}
  };

std::map<std::string_view, std::function<bool(const char*)>> b_s =
  {
    {"is_dir", fs_is_dir},
    {"is_exe", fs_is_exe},
    {"is_file", fs_is_file},
    {"remove", fs_remove},
    {"is_reserved", fs_is_reserved},
    {"is_readable", fs_is_readable},
    {"is_writable", fs_is_writable},
    {"is_symlink", fs_is_symlink},
    {"exists", fs_exists},
    {"is_absolute", fs_is_absolute},
    {"is_char", fs_is_char_device},
    {"mkdir", fs_mkdir},
    {"is_safe", fs_is_safe_name}
  };

constexpr bool strict = false;

// warmup
std::string_view w;
auto t = std::chrono::duration<double>::max();
size_t L=0;
bool b=false;

std::string buf(fs_get_max_path(), '\0');
if (b_s.contains(fname))
  b = b_s[fname](buf.data());
else if (s_.contains(fname))
  L = s_[fname](buf.data(), buf.size());
else if (ssb.contains(fname))
  L = ssb[fname](path.data(), strict, buf.data(), buf.size());
else if (s_s.contains(fname))
  L = s_s[fname](path.data(), buf.data(), buf.size());
else
  {
    std::cerr << "Error: unknown function " << fname << "\n";
    return t;
  }
if(L == 0)
  return t;

w = buf;

for (int i = 0; i < n; ++i){
    auto t0 = std::chrono::steady_clock::now();
    if (b_s.contains(fname))
      b = b_s[fname](buf.data());
    else if (s_.contains(fname))
      s_[fname](buf.data(), buf.size());
    else if (ssb.contains(fname))
      ssb[fname](path.data(), strict, buf.data(), buf.size());
    else if (s_s.contains(fname))
      s_s[fname](path.data(), buf.data(), buf.size());

    auto t1 = std::chrono::steady_clock::now();
    t = std::min(t, std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0));
}

if(verbose)
  print_c(t, n, path, fname, w, b);

return t;
}


void print_c(std::chrono::duration<double> t, int n, std::string_view path, std::string_view func, std::string_view w, bool b)
{
  std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t);
  double us = ns.count() / 1000.0;
  std::cout << "C: " << n << " x " << func << "(" << path << ") = ";
  if(w.empty())
    std::cout << b;
  else
    std::cout << w;
  std::cout << ": " << us << " us\n";
}

void print_cpp(std::chrono::duration<double> t, int n, std::string_view path, std::string_view func, std::string_view w, bool b)
{
  std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t);
  double us = ns.count() / 1000.0;
  std::cout << "Cpp: " << n << " x " << func << "(" << path << ") = ";
  if(w.empty())
    std::cout << b;
  else
    std::cout << w;
  std::cout << ": " << us << " us\n";
}



std::chrono::duration<double> bench_cpp(
  [[maybe_unused]] int n,
  [[maybe_unused]] std::string_view path,
  [[maybe_unused]] std::string_view fname,
  [[maybe_unused]] bool verbose)
{

auto t = std::chrono::duration<double>::max();

#if defined(HAVE_CXX_FILESYSTEM) && HAVE_CXX_FILESYSTEM

std::map<std::string_view, std::function<std::string()>> s_ =
  {
    {"compiler", Ffs::compiler},
    {"homedir", Ffs::get_homedir},
    {"exe_path", Ffs::exe_path},
    {"lib_path", Ffs::lib_path}
  };

std::map<std::string_view, std::function<std::optional<std::string>()>> so_ =
  {
    {"tempdir", Ffs::get_tempdir},
    {"cwd", Ffs::get_cwd}
  };

std::map<std::string_view, std::function<std::string(std::string_view)>> s_s =
  {
    {"as_posix", Ffs::as_posix},
    {"expanduser", Ffs::expanduser},
    {"which", Ffs::which},
    {"parent", Ffs::parent},
    {"root", Ffs::root},
    {"stem", Ffs::stem},
    {"suffix", Ffs::suffix},
    {"filename", Ffs::file_name},
    {"normal", Ffs::normal},
    {"lexically_normal", Ffs::lexically_normal},
    {"make_preferred", Ffs::make_preferred},
    {"mkdtemp", Ffs::mkdtemp},
    {"shortname", Ffs::shortname},
    {"longname", Ffs::longname},
    {"getenv", Ffs::get_env}
  };

std::map<std::string_view, std::function<std::optional<std::string>(std::string_view)>> s_so =
  {
    {"perm", Ffs::get_permissions},
    {"read_symlink", Ffs::read_symlink}
  };

std::map<std::string_view, std::function<std::optional<std::string>(std::string_view, bool)>> ssb =
  {
    {"canonical", Ffs::canonical},
    {"resolve", Ffs::resolve}
  };

std::map<std::string_view, std::function<bool(std::string_view)>> b_s =
  {
    {"is_dir", Ffs::is_dir},
    {"is_exe", Ffs::is_exe},
    {"is_file", Ffs::is_file},
    {"remove", Ffs::remove},
    {"is_reserved", Ffs::is_reserved},
    {"is_readable", Ffs::is_readable},
    {"is_writable", Ffs::is_writable},
    {"is_symlink", Ffs::is_symlink},
    {"exists", Ffs::exists},
    {"is_absolute", Ffs::is_absolute},
    {"is_char", Ffs::is_char_device},
    {"mkdir", Ffs::mkdir},
    {"is_safe", Ffs::is_safe_name}
  };

constexpr bool strict = false;

// warmup
std::string h;
bool b;

if (b_s.contains(fname))
  b = b_s[fname](path);
else if (s_.contains(fname))
  h = s_[fname]();
else if (so_.contains(fname))
  h = so_[fname]().value_or("");
else if (ssb.contains(fname))
  h = ssb[fname](path, strict).value_or("");
else if (s_s.contains(fname))
  h = s_s[fname](path);
else if (s_so.contains(fname))
  h = s_so[fname](path).value_or("");
else
  {
    std::cerr << "Error: unknown function " << fname << "\n";
    return t;
  }

for (int i = 0; i < n; ++i)
{
  auto t0 = std::chrono::steady_clock::now();

  if (b_s.contains(fname))
    b = b_s[fname](path);
  else if (s_.contains(fname))
    h = s_[fname]();
  else if (so_.contains(fname))
    h = so_[fname]().value_or("");
  else if (ssb.contains(fname))
    h = ssb[fname](path, strict).value_or("");
  else if (s_s.contains(fname))
    h = s_s[fname](path);
  else if (s_so.contains(fname))
    h = s_so[fname](path).value_or("");

  auto t1 = std::chrono::steady_clock::now();
  t = std::min(t, std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0));
}

if(verbose)
  print_cpp(t, n, path, fname, h, b);

#endif

return t;
}


int main(int argc, char** argv){

if (!fs_is_optimized())
  std::cerr << "WARNING: ffilesystem might not have been compiled with optimizations\n";

int n = 1000;
if(argc > 1)
    n = std::stoi(argv[1]);

std::string_view path;

if(std::vector<char> buf(1000); fs_compiler(buf.data(), buf.size()))
  std::cout << buf.data() << "\n";

std::set<std::string_view, std::less<>> fset;

if(argc > 3)
  fset = {argv[3]};
else
  fset = {"canonical", "resolve", "which", "expanduser", "normal", "cwd", "homedir", "parent", "is_reserved"};


for (std::set<std::string_view, std::less<>> funcs = fset; std::string_view func : funcs)
  {
  std::set <std::string_view, std::less<>> tildef = {"canonical", "resolve", "normal", "expanduser", "parent"};

  if (argc > 2)
    path = argv[2];
  else {
    if (tildef.contains(func))
      path = "~/..";
    else if (func == "which")
      path = (fs_is_windows()) ? "cmake.exe" : "sh";
    else
      path = "";
  }

 bench_c(n, path, func, true);

  if(fs_cpp())
    bench_cpp(n, path, func, true);
}


return EXIT_SUCCESS;

}

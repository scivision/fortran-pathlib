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

std::map<std::string_view, std::function<std::optional<std::string>(std::string_view, bool, bool)>> ssb =
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
    {"is_absolute", Ffs::is_absolute},
    {"is_char", Ffs::is_char_device},
    {"mkdir", Ffs::mkdir},
    {"is_safe", Ffs::is_safe_name}
  };

constexpr bool strict = false;
constexpr bool expand_tilde = false;

// warmup
std::string h;
bool b = false;

if (b_s.contains(fname))
  b = b_s[fname](path);
else if (s_.contains(fname))
  h = s_[fname]();
else if (so_.contains(fname))
  h = so_[fname]().value_or("");
else if (ssb.contains(fname))
  h = ssb[fname](path, strict, expand_tilde).value_or("");
else if (s_s.contains(fname))
  h = s_s[fname](path);
else if (s_so.contains(fname))
  h = s_so[fname](path).value_or("");
else if (fname == "exists")
  b = fs_exists(path);
else
  {
    std::cerr << "Error: unknown function " << fname << "\n";
    return t;
  }
if (!b_s.contains(fname) && h.empty()){
  std::cerr << "Error:Cpp: " << fname << " " << path << " failed on warmup\n";
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
    h = ssb[fname](path, strict, expand_tilde).value_or("");
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

std::cout << fs_compiler() << "\n";

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

 bench_cpp(n, path, func, true);
}


return EXIT_SUCCESS;

}

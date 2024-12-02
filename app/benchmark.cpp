#include <chrono>
#include <string>
#include <algorithm>
#include <iostream>
#include <functional>
#include <cstdlib>
#include <vector>
#include <array>

#if __has_include(<ranges>)
#include <ranges>  // IWYU pragma: keep
#endif

#include "ffilesystem.h"


void print_cpp(std::chrono::duration<double> t, int n, std::string_view path, std::string_view func, std::string_view w, bool b)
{
  std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t);
  double us = static_cast<double>(ns.count()) / 1000.0;
  std::cout << fs_backend() << ": " << n << " x " << func << "(" << path << ") = ";
  if(w.empty())
    std::cout << b;
  else
    std::cout << w;
  std::cout << ": " << us << " us\n";
}


std::chrono::duration<double> bench_cpp(int n, std::string_view path, std::string_view fname, bool verbose)
{

auto t = std::chrono::duration<double>::max();

constexpr bool strict = false;
constexpr bool expand_tilde = false;

// warmup
std::string h;
bool b = false;

if (fname == "canonical")
  h = fs_canonical(path, strict, expand_tilde);
else if (fname == "resolve")
  h = fs_resolve(path, strict, expand_tilde);
else if (fname == "parent")
  h = fs_parent(path);
else if (fname == "file_name")
  h = fs_file_name(path);
else if (fname == "suffix")
  h = fs_suffix(path);
else if (fname == "normal")
  h = fs_normal(path);
else if (fname == "reserved")
  b = fs_is_reserved(path);
else if (fname == "exists")
  b = fs_exists(path);
else if (fname == "is_dir")
  b = fs_is_dir(path);
else if (fname == "is_char")
  b = fs_is_char_device(path);
else if (fname == "is_file")
  b = fs_is_file(path);
else if (fname == "is_symlink")
  b = fs_is_symlink(path);
else if (fname == "read_symlink")
  h = fs_read_symlink(path);
else if (fname == "which")
  h = fs_which(path);
else if (fname == "homedir")
  h = fs_get_homedir();
else if (fname == "expanduser")
  h = fs_expanduser(path);
else if (fname == "cwd")
  h = fs_get_cwd();
else if (fname == "is_reserved")
  b = fs_is_reserved(path);
else
  {
    std::cerr << "Error: unknown function " << fname << "\n";
    return t;
  }

#if __cpp_lib_starts_ends_with  // C++20
if (!fname.starts_with("is") && h.empty()){
  std::cerr << "Error:Cpp: " << fname << " " << path << " failed on warmup\n";
  return t;
}
#endif

for (int i = 0; i < n; ++i)
{
  auto t0 = std::chrono::steady_clock::now();

  if (fname == "canonical")
    h = fs_canonical(path, strict, expand_tilde);
  else if (fname == "resolve")
    h = fs_resolve(path, strict, expand_tilde);
  else if (fname == "parent")
    h = fs_parent(path);
  else if (fname == "file_name")
    h = fs_file_name(path);
  else if (fname == "suffix")
    h = fs_suffix(path);
  else if (fname == "normal")
    h = fs_normal(path);
  else if (fname == "reserved")
    b = fs_is_reserved(path);
  else if (fname == "exists")
    b = fs_exists(path);
  else if (fname == "is_dir")
    b = fs_is_dir(path);
  else if (fname == "is_char")
    b = fs_is_char_device(path);
  else if (fname == "is_file")
    b = fs_is_file(path);
  else if (fname == "is_symlink")
    b = fs_is_symlink(path);
  else if (fname == "read_symlink")
    h = fs_read_symlink(path);
  else if (fname == "which")
    h = fs_which(path);
  else if (fname == "homedir")
    h = fs_get_homedir();
  else if (fname == "expanduser")
    h = fs_expanduser(path);
  else if (fname == "cwd")
    h = fs_get_cwd();
  else if (fname == "is_reserved")
    b = fs_is_reserved(path);
  else
    {
      std::cerr << "Error: unknown function " << fname << "\n";
      return t;
    }

  auto t1 = std::chrono::steady_clock::now();
  t = std::min(t, std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0));
}

if(verbose)
  print_cpp(t, n, path, fname, h, b);

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

std::vector<std::string_view> funcs;
if(argc > 3)
  funcs = {argv[3]};
else
  funcs = {"canonical", "resolve", "which", "expanduser", "normal", "cwd", "homedir", "parent", "file_name", "is_reserved"};

for (std::string_view func : funcs)
  {
  constexpr std::array<std::string_view, 6> tf = {"canonical", "resolve", "normal", "expanduser", "parent", "file_name"};

  if (argc > 2)
    path = argv[2];
  else {
#ifdef __cpp_lib_ranges_contains  // C++23
  if (std::ranges::contains(tf, func))
#elif __cpp_lib_ranges // C++20
  if (std::ranges::find(tf, func) != tf.end())
#else // C++98
  if (std::find(tf.begin(), tf.end(), func) != tf.end())
#endif
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
#include <iostream>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <map>
#include <ctime>
#include <system_error>
#include <variant>
#include <unordered_map>
#include <type_traits>

#include <chrono> // IWYU pragma: keep
// needed to std::format() std::filesystem::file_time_type

#if __has_include(<format>)
#include <format> // IWYU pragma: keep
#endif

#ifdef HAVE_CXX_FILESYSTEM
#include <filesystem>
#endif

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <crtdbg.h>
#endif

#include "ffilesystem.h"


static void no_arg(std::string_view fun){

  std::map<std::string_view, std::function<bool()>> mbool =
  {
    {"optimized", fs_is_optimized},
    {"is_admin", fs_is_admin},
    {"is_bsd", fs_is_bsd},
    {"is_linux", fs_is_linux},
    {"is_macos", fs_is_macos},
    {"is_rosetta", fs_is_rosetta},
    {"is_unix", fs_is_unix},
    {"is_windows", fs_is_windows},
    {"is_mingw", fs_is_mingw},
    {"is_cygwin", fs_is_cygwin},
    {"has_statx", fs_has_statx}
  };

using fs_function = std::function<std::variant<std::string, bool, int, char, long, size_t>()>;

std::unordered_map<std::string_view, fs_function> fs_function_map = {
  {"backend", []() { return fs_backend(); }},
  {"is_wsl", []() { return fs_is_wsl(); }},
  {"pid", []() { return fs_getpid(); }},
  {"pathsep", []() { return fs_pathsep(); }},
  {"cpp_lang", []() { return fs_cpp_lang(); }},
  {"c_lang", []() { return fs_c_lang(); }},
  {"locale", []() { return fs_get_locale_name(); }},
  {"username", []() { return fs_get_username(); }},
  {"hostname", []() { return fs_hostname(); }},
  {"shell", []() { return fs_get_shell(); }},
  {"stdin_tty", []() { return fs_stdin_tty(); }},
  {"arch", []() { return fs_cpu_arch(); }},
  {"profile", []() { return fs_get_profile_dir(); }},
  {"config", []() { return fs_user_config_dir(); }},
  {"libpath", []() { return fs_lib_path(); }},
  {"exepath", []() { return fs_exe_path(); }},
  {"compiler", []() { return fs_compiler(); }},
  {"homedir", []() { return fs_get_homedir(); }},
  {"terminal", []() { return fs_get_terminal(); }},
  {"tempdir", []() { return fs_get_tempdir(); }},
  {"max_path", []() { return fs_get_max_path(); }}
};

  //"random", [](size_t) { return fs_generate_random_alphanumeric_string(16); },

  auto it = fs_function_map.find(fun);
  if (it != fs_function_map.end()) {
    auto result = it->second();
    if (std::holds_alternative<std::string>(result))
      std::cout << std::get<std::string>(result);
    else if (std::holds_alternative<int>(result))
      std::cout << std::get<int>(result);
    else if (std::holds_alternative<bool>(result))
      std::cout << std::get<bool>(result);
    else if (std::holds_alternative<char>(result))
      std::cout << std::get<char>(result);
    else if (std::holds_alternative<long>(result))
      std::cout << std::get<long>(result);
    else if (std::holds_alternative<size_t>(result))
      std::cout << std::get<size_t>(result);
    else
      std::cerr << "Error: unknown return type " << fun << "\n";

    std::cout << "\n";
  } else if (mbool.find(fun) != mbool.end())
    std::cout << mbool[fun]() << "\n";
  else
    std::cerr << "Error: unknown function " << fun << "()\n";

}

static void one_arg(std::string_view fun, std::string_view a1){


  std::error_code ec;

  using fs_one_arg_function = std::function<std::variant<std::string, bool, size_t
  #if uintmax_t != size_t
  , uintmax_t
  #endif
  >(std::string_view)>;

  std::unordered_map<std::string_view, fs_one_arg_function> fs_one_arg_function_map = {
    {"lexically_normal", [](std::string_view a1) { return fs_lexically_normal(a1); }},
    {"make_preferred", [](std::string_view a1) { return fs_make_preferred(a1); }},
    {"parent", [](std::string_view a1) { return fs_parent(a1); }},
    {"suffix", [](std::string_view a1) { return fs_suffix(a1); }},
    {"canonical", [](std::string_view a1) { return fs_canonical(a1, true, false); }},
    {"weakly_canonical", [](std::string_view a1) { return fs_canonical(a1, false, false); }},
    {"resolve", [](std::string_view a1) { return fs_resolve(a1, true, false); }},
    {"weakly_resolve", [](std::string_view a1) { return fs_resolve(a1, false, false); }},
    {"hard", [](std::string_view a1) { return fs_hard_link_count(a1); }},
    {"normal", [](std::string_view a1) { return fs_normal(a1); }},
    {"mkdtemp", [](std::string_view a1) { return fs_mkdtemp(a1); }},
    {"type", [](std::string_view a1) { return fs_filesystem_type(a1); }},
    {"is_reserved", [](std::string_view a1) { return fs_is_reserved(a1); }},
    {"is_safe", [](std::string_view a1) { return fs_is_safe_name(a1); }},
    {"long", [](std::string_view a1) { return fs_longname(a1); }},
    {"short", [](std::string_view a1) { return fs_shortname(a1); }},
    {"touch", [](std::string_view a1) { return "touch " + std::string(a1) + " " + std::to_string(fs_touch(a1)); }},
    {"getenv", [](std::string_view a1) { return fs_getenv(a1); }},
    {"realpath", [](std::string_view a1) { return fs_realpath(a1); }},
    {"posix", [](std::string_view a1) { return fs_as_posix(a1); }},
    {"max_component", [](std::string_view a1) { return fs_max_component(a1); }},
    {"mkdir", [](std::string_view a1) { return fs_mkdir(a1); }},
    {"which", [](std::string_view a1) { return fs_which(a1); }},
    {"owner", [](std::string_view a1) { return fs_get_owner_name(a1) + "\n" + fs_get_owner_group(a1); }},
    {"expanduser", [](std::string_view a1) { return fs_expanduser(a1); }},
    {"final_path", [](std::string_view a1) { return fs_win32_final_path(a1); }},
    {"root", [](std::string_view a1) { return fs_root(a1); }},
    {"root_name", [](std::string_view a1) { return fs_root_name(a1); }},
    {"filename", [](std::string_view a1) { return fs_file_name(a1); }},
    {"file_size", [](std::string_view a1) { return fs_file_size(a1); }},
    {"is_absolute", [](std::string_view a1) { return fs_is_absolute(a1); }},
    {"is_exe", [](std::string_view a1) { return fs_is_exe(a1); }},
    {"is_case_sensitive", [](std::string_view a1) { return fs_is_case_sensitive(a1); }},
    {"is_dir", [](std::string_view a1) { return fs_is_dir(a1); }},
    {"is_char", [](std::string_view a1) { return fs_is_char_device(a1); }},
    {"is_file", [](std::string_view a1) { return fs_is_file(a1); }},
    {"is_symlink", [](std::string_view a1) { return fs_is_symlink(a1); }},
    {"is_readable", [](std::string_view a1) { return fs_is_readable(a1); }},
    {"is_writable", [](std::string_view a1) { return fs_is_writable(a1); }},
    {"perm", [](std::string_view a1) { return fs_get_permissions(a1); }},
    {"read_symlink", [](std::string_view a1) { return fs_read_symlink(a1); }},
    {"stem", [](std::string_view a1) { return fs_stem(a1); }},
    {"exists", [](std::string_view a1) { return fs_exists(a1); }},
    {"space", [](std::string_view a1) { return fs_space_available(a1); }},
    {"absolute", [](std::string_view a1) { return fs_absolute(a1, true); }},
    {"get_cwd", [](std::string_view) { return fs_get_cwd(); }},
    {"is_empty", [](std::string_view a1) { return fs_is_empty(a1); }},
    {"remove", [](std::string_view a1) { return fs_remove(a1); }}
  };

  auto it = fs_one_arg_function_map.find(fun);
  if (it != fs_one_arg_function_map.end()) {
    auto r = it->second(a1);
    if (std::holds_alternative<std::string>(r))
      std::cout << std::get<std::string>(r);
    else if (std::holds_alternative<bool>(r))
      std::cout << std::get<bool>(r);
    else if (std::holds_alternative<size_t>(r))
      std::cout << std::get<size_t>(r);
    else if (std::holds_alternative<uintmax_t>(r))
      std::cout << std::get<uintmax_t>(r);
    else
      std::cerr << "Error: unknown return type " << fun << "\n";

    std::cout << "\n";
  } else if (fun == "modtime"){

#if defined(HAVE_CXX_FILESYSTEM) && defined(__cpp_lib_format) // C++20
    const auto t = fs_get_modtime_fs(a1);
    if(t)
      std::cout << std::format("{}\n", t.value());
#else
    const std::time_t t = fs_get_modtime(a1);
    #if defined(_MSC_VER)
      std::string buf(26, '\0');
      ctime_s(buf.data(), buf.size(), &t);
      std::cout << buf << "\n";
    #else
      std::cout << std::ctime(&t) << "\n"; // NOSONAR
    #endif
#endif
  } else if (fun == "fs_modtime")
    std::cout << fs_get_modtime(a1) << "\n";
  else if (fun == "chdir" || fun == "set_cwd") {
    auto cwd = fs_get_cwd();
    if(!cwd.empty()){
      std::cout << "cwd: " << cwd << "\n";
      if(fs_set_cwd(a1)){
        cwd = fs_get_cwd();
        if(!cwd.empty())
          std::cout << "new cwd: " << cwd << "\n";
        else
          std::cerr << "ERROR get_cwd() after chdir\n";
      }
    } else {
      std::cerr << "ERROR get_cwd() before chdir\n";
    }
  } else if (fun == "ls") {
#ifdef HAVE_CXX_FILESYSTEM
    for (auto const& dir_entry : std::filesystem::directory_iterator{fs_expanduser(a1)}){
      std::filesystem::path p = dir_entry.path();
      std::cout << p;
      if (const auto &s = std::filesystem::file_size(p, ec); s && !ec)
        std::cout << " " << s;

      std::cout << " " << fs_get_permissions(p.generic_string()) << "\n";
    }
#else
      std::cerr << "ERROR: ls requires C++17 filesystem\n";
#endif
  } else {
    std::cerr << fun << " requires more than 1 argument or is unknown function\n";
  }
}


static void two_arg(std::string_view fun, std::string_view a1, std::string_view a2)
{
  using fs_two_arg_function = std::function<std::variant<std::string, bool>(std::string_view, std::string_view)>;

  std::unordered_map<std::string_view, fs_two_arg_function> fs_two_arg_function_map = {
    {"same", [](std::string_view a1, std::string_view a2) { return fs_equivalent(a1, a2); }},
    {"join", [](std::string_view a1, std::string_view a2) { return fs_join(a1, a2); }},
    {"with_suffix", [](std::string_view a1, std::string_view a2) { return fs_with_suffix(a1, a2); }},
    {"is_subdir", [](std::string_view a1, std::string_view a2) { return fs_is_subdir(a1, a2); }},
    {"relative", [](std::string_view a1, std::string_view a2) { return fs_relative_to(a1, a2); }},
    {"proximate", [](std::string_view a1, std::string_view a2) { return fs_proximate_to(a1, a2); }},
    {"setenv", [](std::string_view a1, std::string_view a2) { return fs_setenv(a1, a2); }},
    {"copy", [](std::string_view a1, std::string_view a2) { return fs_copy_file(a1, a2, false); }},
    {"create_symlink", [](std::string_view a1, std::string_view a2) { return fs_create_symlink(a1, a2); }},
    {"absolute", [](std::string_view a1, std::string_view a2) { return fs_absolute(a1, a2, true); }},
    {"rename", [](std::string_view a1, std::string_view a2) { return fs_rename(a1, a2); }}
  };

  auto it = fs_two_arg_function_map.find(fun);
  if (it != fs_two_arg_function_map.end()) {
    auto r = it->second(a1, a2);
    if (std::holds_alternative<std::string>(r))
      std::cout << std::get<std::string>(r);
    else if (std::holds_alternative<bool>(r))
      std::cout << std::get<bool>(r);

    std::cout << "\n";
  } else
    std::cerr << fun << " requires more than 2 arguments or is unknown function\n";

}

static void four_arg(std::string_view fun, std::string_view a1, std::string_view a2, std::string_view a3, std::string_view a4){
  if (fun == "set_perm"){
    int r = std::stoi(a2.data());
    int w = std::stoi(a3.data());
    int x = std::stoi(a4.data());

    auto p = fs_get_permissions(a1);
    if(p.empty())
      std::cerr << "ERROR get_permissions(" << a1 << ") before chmod\n";
    else
      std::cout << "before chmod " << a1 << " " << p << "\n";

    fs_set_permissions(a1, r, w, x);

    if(p.empty())
      std::cerr << "ERROR get_permissions(" << a1 << ") after chmod\n";
    else
      std::cout << "after chmod " << a1 << " " << p << "\n";

  } else {
    std::cerr << fun << " requires more than 4 arguments or is unknown function\n";
  }

}


int main(){
#ifdef _MSC_VER
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

const std::string cwd = fs_get_cwd();

std::cout << "Ffilesystem. Backend: " << fs_backend() << "\n";
std::cout << "maximum path length: " << fs_get_max_path() << "\n";
std::cout << "maximum path component length: " << fs_max_component(cwd) << "\n";
std::cout << "current working directory (CWD): " << cwd << "\n";
std::cout << "Compiler: " << fs_compiler() << "\n";
std::cout << "Optimized: " << fs_is_optimized() << " Trace: " << FS_TRACE << "\n";
std::cout << "CPU arch: " << fs_cpu_arch() << "\n";
std::cout << "C++ standard " << fs_cpp_lang() << "\n";
std::cout << "C standard " << fs_c_lang() << "\n";
std::cout << "libcpp: " << fs_libcxx() << "\n";
std::cout << "libc: " << fs_libc() << "\n";
std::cout << "Username: " << fs_get_username() << "\n";
std::cout << "Hostname: " << fs_hostname() << "\n";
std::cout << "Shell: " << fs_get_shell() << "\n";
std::cout << "Terminal: " << fs_get_terminal() << "\n";
std::cout << "Homedir: " << fs_get_homedir() << "\n";

// doesn't work usefully on Cygwin
if(!fs_is_cygwin())
  std::cout << "CWD filesystem Type: " << fs_filesystem_type(cwd) << "\n";

// commented out  because some systems (Cygwin) silently exit with error code 0 on std::locale("") call
//std::cout << "Locale: " << fs_get_locale_name() << "\n";

if(fs_is_admin())
  std::cerr << "WARNING: running as admin / sudo" << std::endl;

std::cout << std::endl;  // flush for CI etc.

while (true){

  std::string inp;

  std::cout << "Ffs> ";

  std::getline(std::cin, inp);

  // "\x04" is Ctrl-D on Windows.
  // EOF for non-Windows
  if (std::cin.eof() || inp ==
#ifdef __cpp_named_character_escapes
  "\x{04}"
#else
  "\x04"
#endif
  || inp == "q" || inp == "quit" || inp == "exit")
    break;

  // split variable inp on space-delimiters
  constexpr char delimiter = ' ';
  std::string::size_type pos = 0;
  std::vector<std::string> args;
  // NOTE: loop getline() instead?
  while ((pos = inp.find(delimiter)) != std::string::npos) {
      args.push_back(inp.substr(0, pos));
      inp.erase(0, pos + 1);  // + 1 as delimiter is 1 char
  }
  // last argument
  args.push_back(inp);

  if(args.empty())
    continue;

  const std::vector<std::string>::size_type argc = args.size();

  switch (argc){
  case 1:
    no_arg(args.at(0));
    break;
  case 2:
    one_arg(args.at(0), args.at(1));
    break;
  case 3:
    two_arg(args.at(0), args.at(1), args.at(2));
    break;
  case 4:
    std::cerr << "no 3 argument functions currently\n";
    break;
  case 5:
    four_arg(args.at(0), args.at(1), args.at(2), args.at(3), args.at(4));
    break;
  default:
    std::cerr << "too many arguments " << argc << "\n";
  }


}

return EXIT_SUCCESS;

}

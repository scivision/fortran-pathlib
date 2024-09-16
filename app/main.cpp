#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <ctime>

#include <chrono> // needed to std::format() std::filesystem::file_time_type

#if __has_include(<format>)
#include <format>
#endif

#include <filesystem>

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <crtdbg.h>
#endif

#include "ffilesystem.h"


static void no_arg(std::string_view fun){

  // map string to function call using std::map std::function

  std::map<std::string_view, std::function<bool()>> mbool =
  {
    {"cpp", fs_cpp},
    {"optimized", fs_is_optimized},
    {"is_admin", fs_is_admin},
    {"is_bsd", fs_is_bsd},
    {"is_linux", fs_is_linux},
    {"is_macos", fs_is_macos},
    {"is_unix", fs_is_unix},
    {"is_windows", fs_is_windows},
    {"is_mingw", fs_is_mingw},
    {"is_cygwin", fs_is_cygwin}
  };

  std::map<std::string_view, std::function<std::string()>> mstring =
  {
    {"compiler", Ffs::compiler},
    {"shell", Ffs::get_shell},
    {"terminal", Ffs::get_terminal},
    {"homedir", Ffs::get_homedir},
    {"profile_dir", Ffs::get_profile_dir},
    {"hostname", Ffs::get_hostname},
    {"username", Ffs::get_username},
    {"user_config_dir", Ffs::user_config_dir},
    {"exe_path", Ffs::exe_path},
    {"lib_path", Ffs::lib_path}
  };

  std::map<std::string_view, std::function<std::optional<std::string>()>> mostring =
  {
    {"tempdir", Ffs::get_tempdir},
    {"cwd", Ffs::get_cwd}
  };

  std::map<std::string_view, std::function<int()>> mint =
  {
    {"is_wsl", fs_is_wsl},
    {"pid", fs_getpid}
  };

  std::map<std::string_view, std::function<char()>> mchar =
  {
    {"pathsep", fs_pathsep}
  };

  std::map<std::string_view, std::function<long()>> mlong =
  {
    {"lang", fs_lang}
  };

  if (mbool.contains(fun))
    std::cout << mbool[fun]() << "\n";
  else if (mstring.contains(fun))
    std::cout << mstring[fun]() << "\n";
  else if (mostring.contains(fun))
    std::cout << mostring[fun]().value_or("") << "\n";
  else if (mint.contains(fun))
    std::cout << mint[fun]() << "\n";
  else if (mchar.contains(fun))
    std::cout << mchar[fun]() << "\n";
  else if (mlong.contains(fun))
    std::cout << mlong[fun]() << "\n";
  else if (fun == "cpu_arch")
    std::cout << Ffs::cpu_arch() << "\n";
  else if (fun == "max_path")
    std::cout << fs_get_max_path() << "\n";
  else if (fun == "loadavg")
#if defined(__cpp_lib_format)
    std::cout << std::format("{:0.3f}\n", fs_cpu_loadavg());
#else
    std::cout << fs_cpu_loadavg() << "\n";
#endif
  else
    std::cerr << fun << " not a known function\n";

}

static void one_arg(std::string_view fun, std::string_view a1){

  std::map<std::string_view, std::function<bool(std::string_view)>> mbool =
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

  std::map<std::string_view, std::function<std::string(std::string_view)>> mstring =
  {
    {"as_posix", Ffs::as_posix},
    {"expanduser", Ffs::expanduser},
    {"realpath", Ffs::realpath},
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
    {"owner_name", Ffs::get_owner_name},
    {"owner_group", Ffs::get_owner_group},
    {"shortname", Ffs::shortname},
    {"longname", Ffs::longname},
    {"getenv", Ffs::get_env},
    {"type", Ffs::filesystem_type}
  };

    std::map<std::string_view, std::function<std::optional<std::string>(std::string_view)>> mostring =
  {
    {"perm", Ffs::get_permissions},
    {"read_symlink", Ffs::read_symlink}
  };

  std::map<std::string_view, std::function<std::optional<std::string>(std::string_view, bool, bool)>> mstrb =
  {
    {"canonical", Ffs::canonical},
    {"resolve", Ffs::resolve}
  };

  std::map<std::string_view, std::function<std::optional<std::string>(std::string_view, bool, bool)>> mstrbw =
  {
    {"weakly_canonical", Ffs::canonical}
  };

  std::map<std::string_view, std::function<std::optional<uintmax_t>(std::string_view)>> mmax =
  {
    {"size", Ffs::file_size},
    {"space", Ffs::space_available}
  };

  std::map<std::string_view, std::function<size_t(std::string_view)>> smax =
  {
    {"max_component", Ffs::max_component}
  };

  std::map<std::string_view, std::function<void(std::string_view)>> mvoid =
  {
    {"touch", Ffs::touch}
  };

  std::error_code ec;

  if(mbool.contains(fun))
    std::cout << mbool[fun](a1) << "\n";
  else if (mstring.contains(fun))
    std::cout << mstring[fun](a1) << "\n";
  else if (mostring.contains(fun))
    std::cout << mostring[fun](a1).value_or("") << "\n";
  else if (mstrb.contains(fun))
    std::cout << mstrb[fun](a1, true, false).value_or("") << "\n";
  else if (mstrbw.contains(fun))
    std::cout << mstrbw[fun](a1, false, false).value_or("") << "\n";
  else if (mmax.contains(fun))
    std::cout <<  mmax[fun](a1).value_or(0) << "\n";
  else if (smax.contains(fun))
    std::cout << smax[fun](a1) << "\n";
  else if (mvoid.contains(fun))
    mvoid[fun](a1);
  else if (fun == "modtime"){
#if defined(__cpp_lib_format)
    const auto t = Ffs::get_modtime(a1);
    if(t)
      std::cout << std::format("{}\n", t.value());
#else
    const auto t = fs_get_modtime(a1.data());
    std::cout << std::ctime(&t) << "\n"; // NOSONAR
#endif
  } else if (fun == "fs_modtime")
    std::cout << fs_get_modtime(a1.data()) << "\n";
  else if (fun == "chdir" || fun == "set_cwd") {
    auto cwd = Ffs::get_cwd();
    if(cwd){
      std::cout << "cwd: " << cwd.value() << "\n";
      if(Ffs::chdir(a1)){
        cwd = Ffs::get_cwd();
        if(cwd)
          std::cout << "new cwd: " << cwd.value() << "\n";
        else
          std::cerr << "ERROR get_cwd() after chdir\n";
      }
    } else {
      std::cerr << "ERROR get_cwd() before chdir\n";
    }
  } else if (fun == "ls") {
    for (auto const& dir_entry : std::filesystem::directory_iterator{Ffs::expanduser(a1)}){
      std::filesystem::path p = dir_entry.path();
      std::cout << p;
      if (const auto &s = std::filesystem::file_size(p, ec); s && !ec)
        std::cout << " " << s;

      std::cout << " " << Ffs::get_permissions(p.generic_string()).value_or("") << "\n";
    }
  } else {
    std::cerr << fun << " requires more arguments or is unknown function\n";
  }
}


static void two_arg(std::string_view fun, std::string_view a1, std::string_view a2){

  std::map<std::string_view, std::function<bool(std::string_view, std::string_view)>> mbool =
  {
    {"is_subdir", Ffs::is_subdir},
    {"same", Ffs::equivalent},
    {"setenv", Ffs::set_env}
  };

  std::map<std::string_view, std::function<std::string(std::string_view, std::string_view)>> mstring =
  {
    {"join", Ffs::join},
    {"relative_to", Ffs::relative_to},
    {"with_suffix", Ffs::with_suffix}
  };

  std::map<std::string_view, std::function<bool(std::string_view, std::string_view, bool)>> mvoidb =
  {
    {"copy", Ffs::copy_file}
  };

  if (mbool.contains(fun))
    std::cout << mbool[fun](a1, a2) << "\n";
  else if (mstring.contains(fun))
    std::cout << mstring[fun](a1, a2) << "\n";
  else if (mvoidb.contains(fun))
    mvoidb[fun](a1, a2, false);
  else if (fun == "create_symlink")
    Ffs::create_symlink(a1, a2);
  else
    std::cerr << fun << " requires more arguments or is unknown function\n";

}

static void four_arg(std::string_view fun, std::string_view a1, std::string_view a2, std::string_view a3, std::string_view a4){
  if (fun == "set_perm"){
    int r = std::stoi(a2.data());
    int w = std::stoi(a3.data());
    int x = std::stoi(a4.data());

    auto p = Ffs::get_permissions(a1);
    if(!p)
      std::cerr << "ERROR get_permissions(" << a1 << ") before chmod\n";
    else
      std::cout << "before chmod " << a1 << " " << p.value() << "\n";

    Ffs::set_permissions(a1, r, w, x);

    if(!p)
      std::cerr << "ERROR get_permissions(" << a1 << ") after chmod\n";
    else
      std::cout << "after chmod " << a1 << " " << p.value() << "\n";

  } else {
    std::cerr << fun << " requires more arguments or is unknown function\n";
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

if(fs_is_admin())
  std::cerr << "WARNING: running as admin / sudo\n";

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
  case 5:
    four_arg(args.at(0), args.at(1), args.at(2), args.at(3), args.at(4));
    break;
  default:
    std::cerr << "too many arguments " << argc << "\n";
  }


}

return EXIT_SUCCESS;

}

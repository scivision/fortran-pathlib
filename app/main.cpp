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
    {"homedir", Ffs::get_homedir},
    {"cwd", Ffs::get_cwd},
    {"tempdir", Ffs::get_tempdir},
    {"exe_path", Ffs::exe_path},
    {"lib_path", Ffs::lib_path}
  };

  std::map<std::string_view, std::function<int()>> mint =
  {
    {"is_wsl", fs_is_wsl}
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
  else if (mint.contains(fun))
    std::cout << mint[fun]() << "\n";
  else if (mchar.contains(fun))
    std::cout << mchar[fun]() << "\n";
  else if (mlong.contains(fun))
    std::cout << mlong[fun]() << "\n";
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
    {"which", Ffs::which},
    {"parent", Ffs::parent},
    {"root", Ffs::root},
    {"stem", Ffs::stem},
    {"suffix", Ffs::suffix},
    {"filename", Ffs::file_name},
    {"perm", Ffs::get_permissions},
    {"read_symlink", Ffs::read_symlink},
    {"normal", Ffs::normal},
    {"lexically_normal", Ffs::lexically_normal},
    {"make_preferred", Ffs::make_preferred},
    {"mkdtemp", Ffs::mkdtemp},
    {"shortname", Ffs::shortname},
    {"longname", Ffs::longname},
    {"getenv", Ffs::get_env},
    {"type", Ffs::filesystem_type}
  };

  std::map<std::string_view, std::function<std::string(std::string_view, bool)>> mstrb =
  {
    {"canonical", Ffs::canonical},
    {"resolve", Ffs::resolve}
  };

  std::map<std::string_view, std::function<std::string(std::string_view, bool)>> mstrbw =
  {
    {"weakly_canonical", Ffs::canonical},
    {"weakly_resolve", Ffs::resolve}
  };

  std::map<std::string_view, std::function<uintmax_t(std::string_view)>> mmax =
  {
    {"size", Ffs::file_size},
    {"space", Ffs::space_available}
  };

  std::map<std::string_view, std::function<void(std::string_view)>> mvoid =
  {
    {"touch", Ffs::touch}
  };

  if(mbool.contains(fun))
    std::cout << mbool[fun](a1) << "\n";
  else if (mstring.contains(fun))
    std::cout << mstring[fun](a1) << "\n";
  else if (mstrb.contains(fun))
    std::cout << mstrb[fun](a1, true) << "\n";
  else if (mstrbw.contains(fun))
    std::cout << mstrbw[fun](a1, false) << "\n";
  else if (mmax.contains(fun))
    std::cout << mmax[fun](a1) << "\n";
  else if (mvoid.contains(fun))
    mvoid[fun](a1);
  else if (fun == "modtime"){
#if defined(__cpp_lib_format)
    auto t = Ffs::get_modtime(a1);
    std::cout << std::format("{}\n", t);
#else
    auto t = fs_get_modtime(a1.data());
    std::cout << std::ctime(&t) << "\n";
#endif
  } else if (fun == "fs_modtime")
    std::cout << fs_get_modtime(a1.data()) << "\n";
  else if (fun == "chdir" || fun == "set_cwd") {
    std::cout << "cwd: " << Ffs::get_cwd() << "\n";
    Ffs::chdir(a1);
    std::cout << "new cwd: " << Ffs::get_cwd() << "\n";
  } else if (fun == "ls") {
    for (auto const& dir_entry : std::filesystem::directory_iterator{Ffs::expanduser(a1)}){
      std::filesystem::path p = dir_entry.path();
      std::cout << p;
      if (Ffs::is_file(p.generic_string()))
        std::cout << " " << Ffs::file_size(p.generic_string());

      std::cout << " " << Ffs::get_permissions(p.generic_string()) << "\n";
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

    std::cout << "before chmod " << a1 << " " << Ffs::get_permissions(a1) << "\n";

    Ffs::set_permissions(a1, r, w, x);

    std::cout << "after chmod " << a1 << " " << Ffs::get_permissions(a1) << "\n";
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
  if (std::cin.eof() || inp == "\x04" || inp == "q" || inp == "quit" || inp == "exit")
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

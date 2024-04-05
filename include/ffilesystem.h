#ifndef FFILESYSTEM_H
#define FFILESYSTEM_H

#ifndef FS_TRACE
#define FS_TRACE 0
#endif


#ifdef __cplusplus

// GCC itself does it this way https://github.com/gcc-mirror/gcc/blob/78b56a12dd028b9b4051422c6bad6260055e4465/libcpp/system.h#L426
#if __has_cpp_attribute(unlikely)
#define FFS_UNLIKELY [[unlikely]]
#define FFS_LIKELY [[likely]]
#endif
#ifndef FFS_UNLIKELY
#define FFS_UNLIKELY
#define FFS_LIKELY
#endif

#include <cstdint>
#include <cstdlib>
#include <algorithm> // std::min
#include <string>
#include <ctime> // time_t

#include <filesystem>

#ifdef __cpp_lib_filesystem

class Ffs
{
public:
  static std::string compiler();
  static std::string get_homedir();
  static std::string get_tempdir();
  static std::string get_cwd();

  static std::string exe_path();
  static std::string lib_path();

  static std::string expanduser(std::string_view);

  static bool is_absolute(std::string_view);
  static bool is_char_device(std::string_view);

  static bool is_dir(std::string_view);
  static bool is_exe(std::string_view);
  static bool is_readable(std::string_view);
  static bool is_writable(std::string_view);
  static bool is_symlink(std::string_view);
  static bool exists(std::string_view);
  static bool is_file(std::string_view);
  static bool is_reserved(std::string_view);
  static bool is_subdir(std::string_view, std::string_view);

  static bool remove(std::string_view);

  static std::string as_posix(std::string_view);

  static std::string normal(std::string_view);
  static std::string lexically_normal(std::string_view);
  static std::string make_preferred(std::string_view);

  static std::string file_name(std::string_view);
  static std::string stem(std::string_view);
  static std::string parent(std::string_view);
  static std::string suffix(std::string_view);
  static std::string root(std::string_view);
  static std::string which(std::string_view);

  static void touch(std::string_view);
  static std::filesystem::file_time_type get_modtime(std::string_view);
  static bool set_modtime(std::string_view);

  static std::string canonical(std::string_view, bool);
  static std::string resolve(std::string_view, bool);

  static std::string read_symlink(std::string_view);
  static std::string get_permissions(std::string_view);

  static uintmax_t file_size(std::string_view);
  static uintmax_t space_available(std::string_view);

  static std::string mkdtemp(std::string_view);

  static std::string shortname(std::string_view);
  static std::string longname(std::string_view);

  static std::string get_env(std::string_view);
  static bool set_env(std::string_view, std::string_view);

  static bool mkdir(std::string_view);
  static bool chdir(std::string_view);

  static bool equivalent(std::string_view, std::string_view);

  static std::string join(std::string_view, std::string_view);
  static std::string relative_to(std::string_view, std::string_view);
  static std::string proximate_to(std::string_view, std::string_view);

  static std::string with_suffix(std::string_view, std::string_view);
  static std::string make_absolute(std::string_view, std::string_view);

  static bool create_symlink(std::string_view, std::string_view);
  static bool copy_file(std::string_view, std::string_view, bool);

  static void set_permissions(std::string_view, int, int, int);

  static bool is_safe_name(std::string_view);

  // Disallow creating an instance of this object
  Ffs() = delete;
};

size_t fs_str2char(std::string_view, char*, size_t);
std::string fs_drop_slash(std::string_view);


#endif // __cpp_lib_filesystem

extern "C" {

#else

#if defined(__has_c_attribute)
// not on same line for GCC < 11
#if __has_c_attribute(maybe_unused)
#define FFS_MUNUSED_C [[maybe_unused]]
#endif
#endif
#ifndef FFS_MUNUSED_C
#define FFS_MUNUSED_C
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

size_t fs_strncpy(const char*, char*, size_t);

#endif


bool fs_cpp();
long fs_lang();
size_t fs_get_max_path();

char fs_pathsep();

bool fs_is_admin();
bool fs_is_bsd();
bool fs_is_macos();
bool fs_is_linux();
bool fs_is_unix();
bool fs_is_windows();
int fs_is_wsl();
bool fs_is_mingw();
bool fs_is_cygwin();

bool fs_is_safe_name(const char*);

void fs_as_posix(char*);

size_t fs_normal(const char*, char*, size_t);

size_t fs_join(const char*, const char*, char*, size_t);
size_t fs_make_absolute(const char*, const char*, char*, size_t);

size_t fs_file_name(const char*, char*, size_t);
size_t fs_stem(const char*, char*, size_t);
size_t fs_parent(const char*, char*, size_t);
size_t fs_suffix(const char*, char*, size_t);
size_t fs_root(const char*, char*, size_t);

size_t fs_with_suffix(const char*, const char*, char*, size_t);

size_t fs_which(const char*, char*, size_t);

bool fs_is_symlink(const char*);
bool fs_create_symlink(const char*, const char*);
size_t fs_read_symlink(const char*, char*, size_t);

bool fs_mkdir(const char*);
bool fs_exists(const char*);

bool fs_is_absolute(const char*);
bool fs_is_char_device(const char*);
bool fs_is_dir(const char*);
bool fs_is_file(const char*);
bool fs_is_exe(const char*);
bool fs_is_readable(const char*);
bool fs_is_writable(const char*);
bool fs_is_reserved(const char*);
bool fs_is_subdir(const char*, const char*);

bool fs_set_permissions(const char*, int, int, int);

size_t fs_get_permissions(const char*, char*, size_t);

bool fs_remove(const char*);

size_t fs_canonical(const char*, bool, char*, size_t);
size_t fs_resolve(const char*, bool, char*, size_t);

bool fs_equivalent(const char*, const char*);
bool fs_copy_file(const char*, const char*, bool);

size_t fs_relative_to(const char*, const char*, char*, size_t);
size_t fs_proximate_to(const char*, const char*, char*, size_t);

bool fs_touch(const char*);

time_t fs_get_modtime(const char*);
bool fs_set_modtime(const char*);

size_t fs_get_cwd(char*, size_t);
bool fs_set_cwd(const char*);
size_t fs_get_homedir(char*, size_t);
size_t fs_get_tempdir(char*, size_t);

size_t fs_expanduser(const char*, char*, size_t);

uintmax_t fs_file_size(const char*);
uintmax_t fs_space_available(const char*);

size_t fs_exe_path(char*, size_t);
size_t fs_lib_path(char*, size_t);

size_t fs_compiler(char*, size_t);

bool fs_win32_is_symlink(const char*);

size_t fs_make_tempdir(char*, size_t);

size_t fs_shortname(const char*, char*, size_t);
size_t fs_longname(const char*, char*, size_t);

size_t fs_getenv(const char*, char*, size_t);
bool fs_setenv(const char*, const char*);

#ifdef __cplusplus
}
#endif

#endif

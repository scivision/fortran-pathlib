#ifndef FFILESYSTEM_H
#define FFILESYSTEM_H

#ifndef FS_TRACE
#define FS_TRACE 0
#endif



#ifdef __cplusplus

#if !defined(__has_cpp_attribute)
// this is a C++20 feature, but available in many older compilers
#  define __has_cpp_attribute(x)  0
#endif

// GCC itself does it this way https://github.com/gcc-mirror/gcc/blob/78b56a12dd028b9b4051422c6bad6260055e4465/libcpp/system.h#L426
#if __has_cpp_attribute(unlikely)
#  define FFS_UNLIKELY [[unlikely]]
#  define FFS_LIKELY [[likely]]
#else
#  define FFS_UNLIKELY
#  define FFS_LIKELY
#endif

#include <cstdint>
#include <cstdlib>
#include <algorithm> // std::min
#include <string>
#include <string_view>
#include <ctime> // time_t
#include <optional>

#if __has_include(<filesystem>)
#include <filesystem>
#endif

#ifdef __cpp_lib_filesystem

class Ffs
{
public:

  [[nodiscard]] static bool is_subdir(std::string_view, std::string_view);

  [[nodiscard]] static std::string normal(std::string_view);
  [[nodiscard]] static std::string lexically_normal(std::string_view);
  [[nodiscard]] static std::string make_preferred(std::string_view);

  [[nodiscard]] static std::string parent(std::string_view);
  [[nodiscard]] static std::string suffix(std::string_view);

  [[nodiscard]] static std::optional<std::filesystem::file_time_type> get_modtime(std::string_view);

  [[nodiscard]] static std::optional<std::string> canonical(std::string_view, const bool, const bool);
  [[nodiscard]] static std::optional<std::string> resolve(std::string_view, const bool, const bool);

  [[nodiscard]] static bool equivalent(std::string_view, std::string_view);

  [[nodiscard]] static std::string join(std::string_view, std::string_view);
  [[nodiscard]] static std::string relative_to(std::string_view, std::string_view);
  [[nodiscard]] static std::string proximate_to(std::string_view, std::string_view);

  [[nodiscard]] static std::string with_suffix(std::string_view, std::string_view);

  [[nodiscard]] static std::string drop_slash(std::string_view);

  // Disallow creating an instance of this object
  Ffs() = delete;
};

#endif // __cpp_lib_filesystem

// C++ functions available without C++17 filesystem too

[[nodiscard]] std::string::size_type fs_str2char(std::string_view, char*, const std::string::size_type);

void fs_print_error(std::string_view, std::string_view);
bool fs_win32_create_symlink(std::string_view, std::string_view);


[[nodiscard]] std::string fs_absolute(std::string_view, const bool);
[[nodiscard]] std::string fs_absolute(std::string_view, std::string_view, const bool);

[[nodiscard]] std::string fs_mkdtemp(std::string_view);


bool fs_mkdir(std::string_view);

bool fs_set_modtime(std::string_view);
time_t fs_get_modtime(std::string_view);

[[nodiscard]] std::string fs_which(std::string_view);

[[nodiscard]] bool fs_is_reserved(std::string_view);
[[nodiscard]] bool fs_is_safe_name(std::string_view);

[[nodiscard]] std::string fs_longname(std::string_view);
[[nodiscard]] std::string fs_shortname(std::string_view);

[[nodiscard]] std::string fs_hostname();
[[nodiscard]] std::string fs_cpu_arch();

[[nodiscard]] std::string fs_compiler();

[[nodiscard]] std::string fs_getenv(std::string_view);


[[nodiscard]] std::string fs_get_owner_name(std::string_view);
[[nodiscard]] std::string fs_get_owner_group(std::string_view);


[[nodiscard]] std::string fs_to_cygpath(std::string_view);
[[nodiscard]] std::string fs_to_winpath(std::string_view);

[[nodiscard]] std::string fs_get_shell();
[[nodiscard]] std::string fs_get_terminal();
bool fs_setenv(std::string_view, std::string_view);

[[nodiscard]] std::string fs_get_homedir();
[[nodiscard]] std::string fs_get_profile_dir();
[[nodiscard]] std::string fs_user_config_dir();
[[nodiscard]] std::string fs_get_username();

[[nodiscard]] std::string fs_expanduser(std::string_view);

[[nodiscard]] std::string fs_file_name(std::string_view);

bool fs_touch(std::string_view);

[[nodiscard]] std::string::size_type fs_max_component(std::string_view);

[[nodiscard]] std::string fs_as_posix(std::string_view);
[[nodiscard]] std::string fs_lib_path();
[[nodiscard]] std::string fs_exe_path();

[[nodiscard]] std::string fs_realpath(std::string_view);
[[nodiscard]] std::string fs_trim(std::string r);

[[nodiscard]] std::string fs_stem(std::string_view);

[[nodiscard]] bool fs_is_absolute(std::string_view path);
[[nodiscard]] bool fs_is_symlink(std::string_view);
bool fs_create_symlink(std::string_view, std::string_view);

bool fs_set_permissions(std::string_view, int, int, int);

bool fs_copy_file(std::string_view, std::string_view, bool);

[[nodiscard]] bool fs_exists(std::string_view path);
[[nodiscard]] bool fs_is_file(std::string_view);

[[nodiscard]] bool fs_is_exe(std::string_view);

[[nodiscard]] bool fs_is_readable(std::string_view);
[[nodiscard]] bool fs_is_writable(std::string_view);

[[nodiscard]] std::string fs_root(std::string_view path);

[[nodiscard]] std::optional<uintmax_t> fs_space_available(std::string_view);
[[nodiscard]] std::optional<std::string> fs_read_symlink(std::string_view);

[[nodiscard]] std::optional<std::string> fs_get_permissions(std::string_view);

[[nodiscard]] bool fs_is_char_device(std::string_view);
[[nodiscard]] bool fs_is_dir(std::string_view);

[[nodiscard]] std::optional<uintmax_t> fs_file_size(std::string_view);

bool fs_remove(std::string_view);

bool fs_set_cwd(std::string_view);

[[nodiscard]] std::optional<std::string> fs_get_tempdir();
[[nodiscard]] std::optional<std::string> fs_get_cwd();


[[nodiscard]] std::string fs_filesystem_type(std::string_view);

#if defined(_MSC_VER)
int
#else
mode_t
#endif
fs_st_mode(std::string_view path);

// ---------------------------------------------------------------------------

extern "C" {

#else  // C only

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h> // mode_t

#ifndef __has_include
#define __has_include(x) 0
#endif

// mostly internal use functions only for C
size_t fs_strncpy(const char*, char*, const size_t);

#endif  // end C-only


#if !defined(__has_c_attribute)
#  define __has_c_attribute(x)  0
#endif

#if __has_c_attribute(nodiscard)
#  define FFS_NODISCARD [[nodiscard]]
#else
#  define FFS_NODISCARD
#endif


FFS_NODISCARD bool fs_cpp();
FFS_NODISCARD long fs_lang();

FFS_NODISCARD size_t fs_get_max_path();
FFS_NODISCARD size_t fs_max_component(const char*);

FFS_NODISCARD bool fs_is_optimized();

FFS_NODISCARD int fs_getpid();

FFS_NODISCARD char fs_pathsep();
FFS_NODISCARD const char* fs_devnull();

FFS_NODISCARD bool fs_is_admin();
FFS_NODISCARD bool fs_is_android();
FFS_NODISCARD bool fs_is_bsd();
FFS_NODISCARD bool fs_is_macos();
FFS_NODISCARD bool fs_is_linux();
FFS_NODISCARD bool fs_is_unix();
FFS_NODISCARD bool fs_is_windows();
FFS_NODISCARD int fs_is_wsl();
FFS_NODISCARD bool fs_is_mingw();
FFS_NODISCARD bool fs_is_cygwin();
FFS_NODISCARD bool fs_is_rosetta();

FFS_NODISCARD bool fs_is_safe_name(const char*);

void fs_as_posix(char*);
void fs_drop_slash(char*);

FFS_NODISCARD double fs_cpu_loadavg();

size_t fs_normal(const char*, char*, const size_t);

size_t fs_realpath(const char*, char*, const size_t);

size_t fs_join(const char*, const char*, char*, const size_t);
size_t fs_absolute(const char*, const char*, const bool, char*, const size_t);

size_t fs_file_name(const char*, char*, const size_t);
size_t fs_stem(const char*, char*, const size_t);
size_t fs_parent(const char*, char*, const size_t);
size_t fs_suffix(const char*, char*, const size_t);
size_t fs_root(const char*, char*, const size_t);

size_t fs_with_suffix(const char*, const char*, char*, const size_t);

FFS_NODISCARD size_t fs_which(const char*, char*, const size_t);

FFS_NODISCARD bool fs_is_symlink(const char*);
bool fs_create_symlink(const char*, const char*);
size_t fs_read_symlink(const char*, char*, const size_t);

bool fs_mkdir(const char*);
bool fs_exists(const char*);

FFS_NODISCARD bool fs_is_absolute(const char*);
FFS_NODISCARD bool fs_is_char_device(const char*);
FFS_NODISCARD bool fs_is_dir(const char*);
FFS_NODISCARD bool fs_is_file(const char*);
FFS_NODISCARD bool fs_is_exe(const char*);
FFS_NODISCARD bool fs_is_readable(const char*);
FFS_NODISCARD bool fs_is_writable(const char*);
FFS_NODISCARD bool fs_is_reserved(const char*);
FFS_NODISCARD bool fs_is_subdir(const char*, const char*);

bool fs_set_permissions(const char*, int, int, int);

size_t fs_get_permissions(const char*, char*, const size_t);

bool fs_remove(const char*);

FFS_NODISCARD size_t fs_canonical(const char*, const bool, const bool, char*, const size_t);
FFS_NODISCARD size_t fs_resolve(const char*, const bool, const bool, char*, const size_t);

FFS_NODISCARD bool fs_equivalent(const char*, const char*);
bool fs_copy_file(const char*, const char*, bool);

size_t fs_relative_to(const char*, const char*, char*, const size_t);
size_t fs_proximate_to(const char*, const char*, char*, const size_t);

bool fs_touch(const char*);

FFS_NODISCARD time_t fs_get_modtime(const char*);
bool fs_set_modtime(const char*);

size_t fs_get_cwd(char*, const size_t);
bool fs_set_cwd(const char*);

size_t fs_get_homedir(char*, const size_t);
size_t fs_get_profile_dir(char*, const size_t);
size_t fs_user_config_dir(char*, const size_t);
size_t fs_get_username(char*, const size_t);

size_t fs_get_owner_name(const char*, char*, const size_t);
size_t fs_get_owner_group(const char*, char*, const size_t);

size_t fs_get_tempdir(char*, const size_t);

size_t fs_expanduser(const char*, char*, const size_t);

FFS_NODISCARD uintmax_t fs_file_size(const char*);
FFS_NODISCARD uintmax_t fs_space_available(const char*);

size_t fs_exe_path(char*, const size_t);
size_t fs_lib_path(char*, const size_t);

size_t fs_compiler(char*, const size_t);
size_t fs_get_shell(char*, const size_t);
size_t fs_get_terminal(char*, const size_t);

bool fs_win32_create_symlink(const char*, const char*);

size_t fs_mkdtemp(const char*, char*, const size_t);

size_t fs_hostname(char*, const size_t);

size_t fs_shortname(const char*, char*, const size_t);
size_t fs_longname(const char*, char*, const size_t);

size_t fs_getenv(const char*, char*, const size_t);
bool fs_setenv(const char*, const char*);

size_t fs_filesystem_type(const char*, char*, const size_t);

void fs_print_error(const char*, const char*);

size_t fs_to_cygpath(const char*, char*, const size_t);
size_t fs_to_winpath(const char*, char*, const size_t);

size_t fs_cpu_arch(char*, const size_t);

#ifdef __cplusplus
}
#endif

#endif

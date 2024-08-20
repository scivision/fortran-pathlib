#ifdef __cplusplus

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace std {
  namespace filesystem = experimental::filesystem;
}
#else
#error "No C++ <filesystem> header available."
#endif

extern "C" {
#endif

bool has_filename(const char* path);

#ifdef __cplusplus
}
#endif

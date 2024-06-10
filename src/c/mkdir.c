#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // malloc

// preferred import order for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <direct.h> // _mkdir
#ifndef mkdir
#define mkdir(dir, mode) _mkdir(dir)
#endif
#ifndef S_IRWXU
#define S_IRWXU (_S_IREAD | _S_IWRITE | _S_IEXEC)
#endif
#else
#include <unistd.h>
#endif

#include "ffilesystem.h"
#include <cwalk.h>


bool fs_mkdir(const char* path)
{

  if(fs_is_dir(path))
    return true;

  struct cwk_segment segment;
  if(!cwk_path_get_first_segment(path, &segment))
    return false;

  const size_t m = fs_get_max_path();

  char *buf = (char*)malloc(m);
  if(!buf) return false;

  char r[4];
  size_t L = 0;

  if(fs_is_windows())
    r[0] = '\0';
  else
    fs_root(path, r, 4);
    // root may be empty for relative paths

  // printf("TRACE: mkdir(%s) %s %.*s\n", path, buf, (int)segment.size, segment.begin);
  do {
    if(L + segment.size + 1 >= m){
      fprintf(stderr, "ERROR:ffilesystem:mkdir: path too long %s\n", path);
      free(buf);
      return false;
    }
    L += snprintf(buf+L, m-L, "%s%.*s", r, (int)segment.size, segment.begin);
    if(r[0] != '/') {
      r[0] = '/';
      r[1] = '\0';
    }

    if(FS_TRACE) printf("TRACE: mkdir(%s) %.*s %zu %zu\n", buf, (int)segment.size, segment.begin, L, strlen(buf));

    if(mkdir(buf, S_IRWXU) && !(errno == EEXIST || errno == EACCES)) {
      fprintf(stderr, "ERROR:ffilesystem:mkdir: %s => %s\n", buf, strerror(errno));
      free(buf);
      return false;
    }

  } while(cwk_path_get_next_segment(&segment));

  free(buf);

  return fs_is_dir(path);
}


size_t fs_make_tempdir(char* result, const size_t buffer_size)
{
  char tmpl[] = "tmp.XXXXXX";
  const char* tmp;

#ifdef _WIN32
  tmp = _mktemp(tmpl);
  if(!tmp){
    fprintf(stderr, "ERROR:filesystem:fs_make_tempdir:_mktemp: could not generate tempdir name %s\n", strerror(errno));
    return 0;
  }
  if(!fs_get_tempdir(result, buffer_size))
    return 0;

  const size_t L = fs_join(result, tmp, result, buffer_size);
  if(L == 0)
    return 0;

  fs_mkdir(result);
  return L;
#else
  tmp = mkdtemp(tmpl);
  /* Linux: stdlib.h  macOS: unistd.h */
  if (!tmp){
    fprintf(stderr, "ERROR:filesystem:fs_make_tempdir:mkdtemp: could not create temporary directory %s\n", strerror(errno));
    return 0;
  }

  return fs_strncpy(tmp, result, buffer_size);
#endif

}

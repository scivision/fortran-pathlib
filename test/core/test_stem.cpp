#include <iostream>
#include <string>
#include <cstdlib>

#include "ffilesystem.h"
#include "ffilesystem_test.h"


int main() {
  std::string s;

  s = fs_stem("");
  if (s != "")
    err("stem empty: " + s);

  s = fs_stem("stem.a.b");
  if (s != "stem.a")
    err("stem multi ext: " + s);

  s = fs_stem(s);
  if (s != "stem")
    err("stem repeated: " + s);

  s = fs_stem("stem");
  if (s != "stem")
    err("stem idempotent: " + s);

  s = fs_stem(".stem");
  if (s != ".stem")
    err("stem leading dot filename idempotent: " + s);

  s = fs_stem("./.stem");
  if (s != ".stem")
    err("stem leading dot filename cwd: " + s);

  s = fs_stem(".stem.txt");
  if (s != ".stem")
    err("stem leading dot filename w/ext: " + s);

  s = fs_stem("./.stem.txt");
  if (s != ".stem")
    err("stem leading dot filename w/ext and cwd: " + s);

  s = fs_stem("../.stem.txt");
  if (s != ".stem")
    err("stem leading dot filename w/ext up: " + s);

  s = fs_stem("stem.");
  if (s != "stem")
    err("stem trailing dot filename idempotent: " + s);

  s = fs_stem("a/..");
  if (s != "..")
    err("stem parent directory: " + s);

  s = fs_stem("a/../");
  if (s != "")
    err("stem parent directory trailing slash: " + s);

  s = fs_stem("a/.");
  if (s != ".")
    err("stem parent directory .: " + s);

  if (fs_is_windows()) {
    std::string p = "C:\\a\\ball.text";
    s = fs_stem(p);
    if (s != "ball")
      err("stem(" + p + ") windows path: " + s + " != ball");
  }

  s = fs_stem("日本語.日本語");
  if (s != "日本語")
    err("stem utf8: " + s);

  s = fs_stem("some space.txt");
  if (s != "some space")
    err("stem space: " + s);

  ok_msg("stem C++");

  return EXIT_SUCCESS;
}

// brief standalone statx() example to test compiler support

#define _DEFAULT_SOURCE

#include <iostream>
#include <cstdlib>
#include <string_view>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h> // statx(), stat()

#include <fcntl.h> // AT_* constants


int main(int argc, char *argv[]){

 std::string_view path = (argc > 1) ? argv[1] : ".";

 struct statx s;

  if( statx(AT_FDCWD, path.data(), AT_NO_AUTOMOUNT, STATX_MTIME, &s) == 0 ) {
    std::time_t t = s.stx_mtime.tv_sec;
    std::cout << path << " mod time: " << std::ctime(&t) << "\n";
  } else {
    std::cerr << "ERROR: statx() get_modtime " << path << "\n";
    return EXIT_FAILURE;
  }

return EXIT_SUCCESS;
}

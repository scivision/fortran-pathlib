#include "ffilesystem.h"

#include <cstdlib>
#include <iostream>

int main(){

  size_t skip = 0;
  size_t fail = 0;

  double load = fs_cpu_loadavg();
  if (load <= 0.0){
    std::cerr << "CPU load average not implemented on this platform\n";
    skip++;
  }
  else
    std::cout << "OK: CPU load average: " << load << "\n";

  const auto max_path = fs_get_max_path();

  std::string hostname = fs_hostname();
  if (hostname.empty()){
    std::cerr << "ERROR: failed to get hostname\n";
    fail++;
  }
  else if (hostname.length() == max_path){
    std::cerr << "ERROR: hostname has blank space\n";
    fail++;
  }
  else
    std::cout << "OK: hostname: " << hostname << "\n";

  std::string compiler = fs_compiler();
  if (compiler.empty()){
    std::cerr << "unknown compiler\n";
    skip++;
  }
  else if (compiler.length() == max_path){
    std::cerr << "ERROR: compiler has blank space\n";
    fail++;
  }
  else if (compiler.find("%") != std::string::npos){
    std::cerr << "ERROR: fs_compiler has formatting problem\n";
    fail++;
  }
  else
    std::cout << "OK: compiler: " << compiler << "\n";

  std::cout << "skip: " << skip << " fail: " << fail << "\n";

  if(fail)
    return EXIT_FAILURE;
  if(skip)
    return 77;

  return EXIT_SUCCESS;
}

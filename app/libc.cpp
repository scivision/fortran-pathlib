// print compile-time and runtime standard C library version

#include <iostream>

#if defined(__UCLIBC__)
#include <features.h>
#elif defined(__GLIBC__)
#include <gnu/libc-version.h>
#endif

std::string get_libc_version(){

std::string v;

#if defined(__UCLIBC__)
v = "uClibc: " + std::string(__UCLIBC__);
#elif defined(__GLIBC__)
v = "GCC: runtime " + std::string(gnu_get_libc_version()) + ", compile-time " + std::to_string(__GLIBC__) + "." + std::to_string(__GLIBC_MINOR__);
#elif defined(__apple_build_version__)
v = "Apple";
#endif

return v;
}

int main(){
    std::cout << get_libc_version() << std::endl;
    return 0;
}

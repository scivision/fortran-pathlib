#include <chrono>
#include <string_view>

std::chrono::duration<double> bench_c(int, std::string_view, std::string_view, bool);

std::chrono::duration<double> bench_cpp(int, std::string_view, std::string_view, bool);

void print_c(std::chrono::duration<double>, int, std::string_view, std::string_view, std::string_view, bool b);
void print_cpp(std::chrono::duration<double>, int, std::string_view, std::string_view, std::string_view, bool b);

#include "ffilesystem.h"

#include <iostream>
#include <algorithm>            // for generate_n
#include <array>                // for array
#include <functional>           // for ref
#include <iterator>             // for begin, end
#include <string>
#include <system_error>         // for error_code

#if defined(HAVE_MERSENNE_TWISTER)
#include <random>
#endif


bool Ffs::mkdir(std::string_view path)
{
  std::error_code ec;
  // old MacOS return false even if directory was created
  if (std::filesystem::create_directories(path, ec) || (!ec && Ffs::is_dir(path))) FFS_LIKELY
    return true;

  std::cerr << "ERROR:Ffs:mkdir(" << path << ") " << ec.message() << "\n";
  return false;
}

#if defined(__cpp_deduction_guides) && defined(HAVE_MERSENNE_TWISTER)
// CTAD C++17 random string generator
// https://stackoverflow.com/a/444614
// https://en.cppreference.com/w/cpp/language/class_template_argument_deduction

template <typename T = std::mt19937>
static auto fs_random_generator() -> T {
    auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
    auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
    auto seed = std::array<std::seed_seq::result_type, seed_len>();
    auto dev = std::random_device();
    std::generate_n(std::begin(seed), seed_len, std::ref(dev));
    auto seed_seq = std::seed_seq(std::begin(seed), std::end(seed));
    return T{seed_seq};
}


static std::string fs_generate_random_alphanumeric_string(const std::string::size_type len)
{
    constexpr std::string_view chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    thread_local auto rng = fs_random_generator<>();
    auto dist = std::uniform_int_distribution{{}, chars.length() - 1};
    auto result = std::string(len, '\0');
    std::generate_n(std::begin(result), len, [&]() { return chars[dist(rng)]; });
    return result;
}
#endif


std::string Ffs::mkdtemp(std::string_view prefix)
{
  // make unique temporary directory starting with prefix
#if defined(__cpp_deduction_guides) && defined(HAVE_MERSENNE_TWISTER)
  std::error_code ec;
  std::filesystem::path t;
  constexpr std::string::size_type Lname = 16;  // arbitrary length for random string
  const std::filesystem::path temp = std::filesystem::temp_directory_path(ec);

  if(!ec) FFS_LIKELY
  {
    do {
      t = (temp / (prefix.data() + fs_generate_random_alphanumeric_string(Lname)));
      if(FS_TRACE) std::cout << "TRACE:make_tempdir: " << t << "\n";
    } while (std::filesystem::is_directory(t, ec) && !ec);

    if (std::filesystem::create_directory(t, ec) && !ec) FFS_LIKELY
      return t.generic_string();
  }

  std::cerr << "Ffs::mkdtemp:mkdir: could not create temporary directory " << ec.message() << "\n";
#else
  std::cerr << "Ffs::mkdtemp not available without C++17 CTAD and std::mt19937\n";
#endif
  return {};
}

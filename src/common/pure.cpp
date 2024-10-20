// This file contains pure or lexical functions
// that don't require additional includes

#include "ffilesystem.h"

#include <string>
#include <iostream>
#include <cctype>


std::string Ffs::lexically_normal(std::string_view path){
  return std::filesystem::path(path).lexically_normal().generic_string();
}


std::string Ffs::make_preferred(std::string_view path){
  return std::filesystem::path(path).make_preferred().generic_string();
}


std::string Ffs::with_suffix(std::string_view path, std::string_view new_suffix){
  return std::filesystem::path(path).replace_extension(new_suffix).generic_string();
}

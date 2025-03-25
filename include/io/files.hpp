#ifndef FILES_HPP
#define FILES_HPP

#include "errors/catcher.hpp"
#include <filesystem>

namespace fs = std::filesystem;

bool is_file(const fs::path& path);
std::string read_file(Catcher& catcher, const fs::path& path);

#endif // FILES_HPP

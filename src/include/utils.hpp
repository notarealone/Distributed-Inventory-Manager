#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>
#include <string>
#include <vector>
#include "logger.hpp"
#include "consts.hpp"

std::vector<fs::path> getDirFiles(const std::string& path, Logger& log);
std::vector<std::string> split(const std::string& str, char delimiter);

#endif

#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>
#include <string>
#include <vector>
#include "logger.hpp"
#include "consts.hpp"

std::vector<fs::path> getDirFiles(const std::string& path, Logger& log);

#endif

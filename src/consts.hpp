#ifndef CONSTS_HPP
#define CONSTS_HPP

#include <string>
#include <filesystem>

#define MAX_BUF 1024
#define FD_MAX 5

namespace fs = std::filesystem;

const char SLASH = '/';
const char SPACE_SEPARETOR = ' ';
const std::string OUTPUT = "output";
const std::string INPUT = "input";
const std::string OUT_STORE = "store.out";
const std::string OUT_PART = "part.out";
const std::string OUT_PROFIT = "profit.out";
const std::string PIPE_PATH = "namedPipes";

#endif

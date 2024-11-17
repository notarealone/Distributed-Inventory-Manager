#include "include/utils.hpp"

using namespace std;

vector<fs::path> getDirFiles(const string& path, Logger& log) {
    vector<fs::path> files;

    if (fs::exists(path) && fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            files.push_back(entry.path());
        }
        log.logInfo("Files found in directory: " + path);
    } else {
        log.logError("Directory does not exist or is not a directory: " + path);
    }

    return files;
}

vector<string> split(const string& str, char delimiter) {
    vector<std::string> tokens;
    istringstream tokenStream(str);
    string token;

    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
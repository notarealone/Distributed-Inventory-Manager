#include <unistd.h>
#include <sys/wait.h>

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "logger.hpp"
#include "ansi-color.hpp"
#include "strops.hpp"
#include "consts.hpp"
#include "utils.hpp"
#include "csv.hpp"

using namespace std;

namespace fs= filesystem;
Logger log("main");


void showParts(vector<string> &parts){
    int k=1;
    log.logInfo("We have " + to_string(parts.size()) + " parts:");
    for(auto& part: parts) {
        log.logInfo(ANSI_GRN +to_string(k) + "- " + ANSI_WHT 
                        + part + ANSI_RST);
        k++;
    }
}

int readParts(const std::string& filename, vector<string>& parts, Logger& logger) {
    CSV partsFile(filename);
    if(partsFile.readCSV() == 0){
        for(auto& part : partsFile.getTable()[0])
            parts.push_back(part);
        logger.logInfo("Parts Founded.");
        return 0;
    }
    else {
        logger.logError("Parts Not Found (Wrong Path)");
        return -1;
    }

}


int main(int argc, const char* argv[]){
    if(argc != 2){
        cerr << ANSI_RED << "correct format: "
                  << ANSI_GRN << "warehouseManager.out" 
                  << ANSI_MAG << " <data folder>\n" << ANSI_RST;
        return EXIT_FAILURE;
    }
    vector<string> parts;
    readParts("./stores/Parts.csv", parts,log);
    showParts(parts);
    
}
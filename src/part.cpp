#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>


#include "consts.hpp"
#include "ansi-color.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "strOperations.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if(argc != 3){
        cerr << ANSI_RED << "Incorrect number of argument for Part.out" << ANSI_RST;
        exit(EXIT_FAILURE);
    }
    string partName = argv[1];
    Logger log("Part " + partName);
    int writeToMainFd = atoi(argv[2]);

    int totalLeftOverQuantity = 0;
    int totalLeftoverMoney = 0;

    
    vector<fs::path> pipes = getDirFiles(PIPE_PATH + SLASH + partName,log);
    for(auto& pipe : pipes){
        int readFd = open(pipe.c_str(), O_RDONLY);
        if (readFd== -1) {
            log.logError("Can't open pipe for reading: " + strOperations::split(pipe.string(),SLASH)[2]);
            exit(EXIT_FAILURE);
        }
        log.logInfo("Open Pipe " + strOperations::split(pipe.string(),SLASH)[2] +" successfully.");
        char buffer_[MAX_BUF];
        int bytesRead = read(readFd, buffer_, MAX_BUF);
        buffer_[bytesRead] = '\0';
        string buffer=string(buffer_);
        close(readFd);
        totalLeftOverQuantity += stoi(strOperations::split(buffer,SPACE_SEPARETOR)[0]);
        totalLeftoverMoney += stoi(strOperations::split(buffer,SPACE_SEPARETOR)[1]);
    }

    string codedData = to_string(totalLeftOverQuantity) + SPACE_SEPARETOR + to_string(totalLeftoverMoney);
    write(writeToMainFd, codedData.c_str(), codedData.size());
    close(writeToMainFd);
    exit(EXIT_SUCCESS); 
}
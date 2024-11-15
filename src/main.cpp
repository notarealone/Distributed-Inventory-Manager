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

void getWantedResources(vector<string>& wantedResources){
    string buffer; 
    cout << ANSI_MAG << "Which resource(s) you want? "<<ANSI_YEL<<"(Names of resource(s) - space separated - in order)"<<ANSI_RST<<endl;
    getline(cin,buffer);
    wantedResources=strops::split(buffer,SPACE_SEPARETOR);
}

int createCitiesProcesses(vector<vector<int>> &citiesToMainPipes,vector<vector<int>> &mainToCitiesPipes)
{
    string cities_names[] = CITIES;
    for(int i = 0; i<CITIES_NUM;i++){
        int pid=fork();
        if(pid<0){
            log.logError("Cannot create child process for city " + cities_names[i]);
            return EXIT_FAILURE;
        }
        else if(pid==0){//child proccess
            char readFd[FD_MAX];
            char writeFd[FD_MAX];
            close(mainToCitiesPipes[i][1]);
            close(citiesToMainPipes[i][0]);
            sprintf(readFd,"%d",mainToCitiesPipes[i][0]);           
            sprintf(writeFd,"%d",citiesToMainPipes[i][1]);
            if(execl(OUT_CITY, OUT_CITY, readFd, writeFd, NULL) == -1){
                log.logError("Cannot execute .out file for city " + cities_names[i]);
                return EXIT_FAILURE;
            }
        }
        else if(pid>0){//parent proccess
            close(citiesToMainPipes[i][1]);
            close(mainToCitiesPipes[i][0]);
        }
    }
    return EXIT_SUCCESS;
}

int createProfitProcess(vector<int> &profitToMainPipe,vector<int> &mainToProfitPipe){
    int pid=fork();
    if(pid<0){
        log.logError("Cannot create child process for Profit ");
        return EXIT_FAILURE;
    }
    else if(pid==0){//child proccess
        char readFd[FD_MAX];
        char writeFd[FD_MAX];
        close(mainToProfitPipe[1]);
        close(profitToMainPipe[0]);
        sprintf(readFd,"%d",mainToProfitPipe[0]);           
        sprintf(writeFd,"%d",profitToMainPipe[1]);
        if(execl(OUT_PROFIT, OUT_PROFIT, readFd, writeFd, NULL) == -1){
            log.logError("Cannot execute .out file for Profit ");
            return EXIT_FAILURE;
        }
    }
    else if(pid>0){//parent proccess
        close(profitToMainPipe[1]);
        close(mainToProfitPipe[0]);
    }
}

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
    
    vector<string> wantedResources;
    getWantedResources(wantedResources);

    //create unnamed pipes between cities and main process
    vector<vector<int>> citiesToMainPipes(CITIES_NUM, vector<int>(2));
    vector<vector<int>> mainToCitiesPipes(CITIES_NUM, vector<int>(2));
    createCitiesProcesses(citiesToMainPipes, mainToCitiesPipes);

    //send Informations to city processes
    string resources_info = "";
    for(int i=0; i<CITIES_NUM; i++){
        if(i == CITIES_NUM-1)
            resources_info += wantedResources[i];
        else
            resources_info += (wantedResources[i] + "$");
    }
    for(int i=0; i<CITIES_NUM; i++)
            write(mainToCitiesPipes[i][1],resources_info.c_str(),resources_info.size()); //info format : resource1$resource$resource3\0
    
    log.logInfo("Information sent to buildings successfully.");
    sleep(2);

    //create unnamed pipe between profit and main process
    vector<int> profitToMainPipe(2);
    vector<int> mainToProfitPipe(2);
    createProfitProcess(profitToMainPipe, mainToProfitPipe);

    //wait for child proccesses to finish
    for (int i = 0; i < CITIES_NUM+1; i++) {
        int status;
        wait(&status);
    }
    log.logInfo("All children proccesses exit successfully.");

    //Combine Results
    

}
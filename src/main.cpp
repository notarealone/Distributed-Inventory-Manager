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

int createStoresProcesses(vector<vector<int>> &storeToMainPipes,vector<vector<int>> &mainToStorePipes,vector<fs::path> &stores)
{
    for(int i = 0; i<stores.size();i++){
        int pid=fork();
        if(pid<0){
            log.logError("Cannot create child process for store " + strops::split(stores[i],SLASH)[1]);
            return EXIT_FAILURE;
        }
        else if(pid==0){//child proccess
            char filePath[MAX_BUF];
            char readFd[FD_MAX];
            char writeFd[FD_MAX];
            sprintf(filePath,"%s",stores[i].string().c_str());
            close(mainToStorePipes[i][1]);
            close(storeToMainPipes[i][0]);
            sprintf(readFd,"%d",mainToStorePipes[i][0]);           
            sprintf(writeFd,"%d",storeToMainPipes[i][1]);
            if(execl(OUT_STORE,OUT_STORE,filePath,readFd,writeFd,NULL)==-1){
                log.logError("Cannot execute .out file for building " + strops::split(stores[i],SLASH)[1]);
                return EXIT_FAILURE;
            }
        }
        else if(pid>0){//parent proccess
            close(storeToMainPipes[i][1]);
            close(mainToStorePipes[i][0]);
        }
    }
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

vector<string> getWantedParts(vector<string> &parts) {
    vector<string> wantedParts;
    int k = 1;

    cout << "We have " << parts.size() << " parts:\n";
    for (auto& part : parts) {
        cout << ANSI_GRN << k << "- " << ANSI_WHT << part << ANSI_RST << endl;
        k++;
    }

    cout << "\nEnter the numbers of the parts you want, separated by spaces (e.g., 1 3 5): ";
    string input;
    getline(cin, input); 

    istringstream stream(input);
    int partNumber;
    while (stream >> partNumber) {
        wantedParts.push_back(parts[partNumber - 1]);
    }
    return wantedParts;
}

int readParts(const std::string& filename, vector<string>& parts) {
    CSV partsFile(filename);
    if(partsFile.readCSV() == 0){
        for(auto& part : partsFile.getTable()[0])
            parts.push_back(part);
        log.logInfo("Parts Founded.");
        return 0;
    }
    else {
        log.logError("Parts Not Found (Wrong Path)");
        return -1;
    }

}

vector<fs::path> getStores(string rootFolderPath) {
    vector<fs::path> csvFiles = getDirFiles(rootFolderPath,log);
    fs::path unwantedFile = fs::path(rootFolderPath) / "Parts.csv";
    vector<fs::path> storesPaths;
    for (const auto& file : csvFiles) {
        if (file != unwantedFile) {
            storesPaths.push_back(file);
        }
    }
    return storesPaths;
}

int main(int argc, const char* argv[]){
    if(argc != 2){
        cerr << ANSI_RED << "correct format: "
                  << ANSI_GRN << "warehouseManager.out" 
                  << ANSI_MAG << " <data folder>\n" << ANSI_RST;
        return EXIT_FAILURE;
    }
    vector<string> parts;
    string partsPath = string(argv[1]) + "/Parts.csv";
    readParts(partsPath, parts);
    vector<string> wantedParts = getWantedParts(parts);
    
    
    vector<fs::path> stores = getStores(argv[1]);

    //create unnamed pipes between stores and main process
    vector<vector<int>> storeToMainPipes(stores.size(), vector<int>(2));
    vector<vector<int>> mainToStorePipes(stores.size(), vector<int>(2));

    for(int i = 0; i<stores.size();i++){
        if(pipe(storeToMainPipes[i].data()) == -1){
            log.logError("Cannot create pipe for store " + to_string(i+1));
            return EXIT_FAILURE;
        }
        if(pipe(mainToStorePipes[i].data()) == -1){
            log.logError("Cannot create pipe for store " + to_string(i+1));
            return EXIT_FAILURE;
        }
    }
    log.logInfo("Pipes between stores and main created successfully.");

    //create stores proccesses

    createStoresProcesses(storeToMainPipes, mainToStorePipes,stores);
    log.logInfo("Stores proccesses created successfully.");


    // //send Informations to city processes
    // string resources_info = "";
    // for(int i=0; i<CITIES_NUM; i++){
    //     if(i == CITIES_NUM-1)
    //         resources_info += wantedResources[i];
    //     else
    //         resources_info += (wantedResources[i] + "$");
    // }
    // for(int i=0; i<CITIES_NUM; i++)
    //         write(mainToCitiesPipes[i][1],resources_info.c_str(),resources_info.size()); //info format : resource1$resource$resource3\0
    
    // log.logInfo("Information sent to buildings successfully.");
    // sleep(2);

    //create unnamed pipe between profit and main process
    // vector<int> profitToMainPipe(2);
    // vector<int> mainToProfitPipe(2);
    // createProfitProcess(profitToMainPipe, mainToProfitPipe);

    // //wait for child proccesses to finish
    // for (int i = 0; i < CITIES_NUM+1; i++) {
    //     int status;
    //     wait(&status);
    // }
    // log.logInfo("All children proccesses exit successfully.");

    //Combine Results
    

}
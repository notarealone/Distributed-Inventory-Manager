#include <unistd.h>
#include <sys/wait.h>

#include <iostream>
#include <vector>
#include <algorithm>

#include "include/logger.hpp"
#include "include/csv.hpp"
#include "include/ansi-color.hpp"
#include "include/consts.hpp"
#include "include/utils.hpp"

using namespace std;

Logger log("main");

void createPartsProcesses(vector<vector<int>> &partsToMainPipes,vector<string> &wantedParts){
    for(int i = 0; i<wantedParts.size();i++){
        sleep(1);
        int pid=fork();
        if(pid<0){
            log.logError("Cannot create child process for part " + wantedParts[i]);
            return ;
        }
        else if(pid==0){//child proccess
            char writeFd[FD_MAX];
            close(partsToMainPipes[i][0]);
            sprintf(writeFd,"%d",partsToMainPipes[i][1]);
            if(execl(OUT_PART.c_str(),OUT_PART.c_str(),wantedParts[i].c_str(),writeFd,NULL)==-1){
                log.logError("Cannot execute .out file for part " + wantedParts[i]);
                return ;
            }
        }
        else if(pid>0){//parent proccess
            close(partsToMainPipes[i][1]);
        }
    }
}

void createStoresProcesses(vector<vector<int>> &storeToMainPipes,vector<vector<int>> &mainToStorePipes,vector<fs::path> &stores)
{
    for(int i = 0; i<stores.size();i++){
        int pid=fork();
        if(pid<0){
            log.logError("Cannot create child process for store " + split(stores[i],SLASH)[1]);
            return ;
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
            if(execl(OUT_STORE.c_str(),OUT_STORE.c_str(),filePath,readFd,writeFd,NULL)==-1){
                log.logError("Cannot execute .out file for building " + split(stores[i],SLASH)[1]);
                return ;
            }
        }
        else if(pid>0){//parent proccess
            close(storeToMainPipes[i][1]);
            close(mainToStorePipes[i][0]);
        }
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
    
    //create unnamed pipe between main and parts
    vector<vector<int>> partsToMainPipes(wantedParts.size(), vector<int>(2));
    for(int i = 0; i<wantedParts.size();i++)
    {
        if(pipe(partsToMainPipes[i].data()) == -1)
        {
            log.logError("Cannot create pipe for part " + wantedParts[i]);
            return EXIT_FAILURE;
        }
    }
    log.logInfo("Pipes between parts and main created successfully.");

    vector<fs::path> stores = getStores(argv[1]);

    //create unnamed pipes between stores and main process
    vector<vector<int>> storeToMainPipes(stores.size(), vector<int>(2));
    vector<vector<int>> mainToStorePipes(stores.size(), vector<int>(2));

    for(int i = 0; i<stores.size();i++)
    {
        if(pipe(storeToMainPipes[i].data()) == -1)
        {
            log.logError("Cannot create pipe for store " + to_string(i+1));
            return EXIT_FAILURE;
        }
        if(pipe(mainToStorePipes[i].data()) == -1)
        {
            log.logError("Cannot create pipe for store " + to_string(i+1));
            return EXIT_FAILURE;
        }
    }
    log.logInfo("Pipes between stores and main created successfully.");

    //create stores proccesses
    createStoresProcesses(storeToMainPipes, mainToStorePipes,stores);
    log.logInfo("Stores proccesses created successfully.");


    //send WantedParts to store processes
    string wantedPartsAsString = "";
    for(int i = 0; i < wantedParts.size(); i++)
    {
            wantedPartsAsString += wantedParts[i] + SPACE_SEPARETOR;
    }
    for(int i = 0; i < stores.size(); i++)
    {
        write(mainToStorePipes[i][1],wantedPartsAsString.c_str(),wantedPartsAsString.size());
    }
    
    log.logInfo("Wanted parts sent to stores successfully.");

    //create parts proccesses   
    createPartsProcesses(partsToMainPipes,wantedParts);
    log.logInfo("Parts proccesses created successfully.");

    //  wait for children proccesses to finish
    for(int i = 0; i<stores.size();i++)
    {
        wait(NULL);
    }

    for(int i = 0; i<wantedParts.size();i++)
    {
        wait(NULL);
    }
    log.logInfo("All children proccesses exit successfully.");


    //read total Quantities and Total Prices from parts
    for(int i=0; i < partsToMainPipes.size() ; i++){
        char buffer[MAX_BUF];
        int bytesRead = read(partsToMainPipes[i][0], buffer, MAX_BUF);
        if (bytesRead <= 0) {
            log.logError("Failed to read from part process: " + wantedParts[i]);
            return EXIT_FAILURE;
        }
        buffer[bytesRead] = '\0';
        string partTotalQuantity = split(buffer,SPACE_SEPARETOR)[0] ;
        string partTotalPrice = split(buffer,SPACE_SEPARETOR)[1];
        cout<< ANSI_MAG << wantedParts[i] << ANSI_RST << "\n" 
            << "\t" << ANSI_GRN << "Total Leftover Quantity = " << ANSI_RST << partTotalQuantity << "\n"
            << "\t" << ANSI_GRN << "Total Leftover Price = " << ANSI_RST << partTotalPrice << endl;
        close(partsToMainPipes[i][0]);
    }

    //read profit from store processes
    int totalProfit = 0;
    for(int i=0; i < storeToMainPipes.size(); i++){
        char buffer[MAX_BUF];
        int bytesRead = read(storeToMainPipes[i][0], buffer, MAX_BUF);
        if (bytesRead <= 0) {
            log.logError("Failed to read from store process");
            return EXIT_FAILURE;
        }
        buffer[bytesRead] = '\0';
        totalProfit += atoi(buffer);
        close(storeToMainPipes[i][0]);
    }
    cout << ANSI_YEL << "Total profit of all stores : " << ANSI_RST << totalProfit << endl;
}
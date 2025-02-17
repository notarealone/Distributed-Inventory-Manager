#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <iostream>

#include "include/consts.hpp"
#include "include/logger.hpp"
#include "include/csv.hpp"
#include "include/utils.hpp"

using namespace std;

Logger logger("Store");

struct Voucher {
    float price;
    int quantity;
};

void calculateProfitAndLeftovers(const vector<vector<string>>& vouchers,const vector<string>& wantedParts,float& totalProfit
    ,vector<int>& leftoverQuantities,vector<float>& leftoverMoney) {
    unordered_map<string, vector<Voucher>> partVouchersMap;

    totalProfit = 0;

    for (const auto& voucher : vouchers) {
        string partName = voucher[0];
        float price = stof(voucher[1]);
        int quantity = stoi(voucher[2]);
        string type = voucher[3];

        if (type == INPUT) 
        {
            partVouchersMap[partName].push_back({price, quantity});
        } 
        else if (type == OUTPUT) 
        {
            auto& inputVouchers = partVouchersMap[partName];

            while (quantity > 0 && !inputVouchers.empty()) {
                auto& topVoucher = inputVouchers[0];

                int usedQuantity = min(quantity, topVoucher.quantity);
                totalProfit += usedQuantity * (price - topVoucher.price);
                quantity -= usedQuantity;

                if (topVoucher.quantity > usedQuantity) {
                    topVoucher.quantity -= usedQuantity;
                }
                else {
                    inputVouchers.erase(inputVouchers.begin());
                }
            }
        }
    }


    for (size_t i = 0; i < wantedParts.size(); ++i) {
        const string& partName = wantedParts[i];
        int totalLeftoverQuantity = 0;
        float totalLeftoverMoney = 0;

        auto& leftOverVouchers = partVouchersMap[partName];
        for(const auto& voucher : leftOverVouchers) {
            totalLeftoverQuantity += voucher.quantity;
            totalLeftoverMoney += voucher.quantity * voucher.price;
        }
        leftoverQuantities.push_back(totalLeftoverQuantity);
        leftoverMoney.push_back(totalLeftoverMoney);
    }
}

void sendDataToPartsProcesses(const string& storeName, const vector<string>& wantedParts,const vector<int>& leftoverQuantities,const vector<float>& leftoverMoney) {
    for (int i = 0; i < wantedParts.size(); ++i) {
        string dirPath = PIPE_PATH + SLASH + wantedParts[i];
        string pipeName = dirPath + SLASH + storeName;

        try {
            if (!fs::exists(dirPath)) {
                fs::create_directories(dirPath);
                logger.logInfo("Directory created: " + dirPath);
            }
        } catch (const fs::filesystem_error& e) {
            logger.logError("Cannot create directory " + dirPath + ": " + e.what());
            return;
        }

        if (mkfifo(pipeName.c_str(), 0666) == -1) {
            logger.logError("Cannot create named pipe for part " + wantedParts[i]);
            return;
        }
        //open pipe for writing
        int writeFd = open(pipeName.c_str(), O_WRONLY);
        if (writeFd == -1) {
            logger.logError("Can't open pipe for writing: " + pipeName);
            return;
        }
        string data = to_string(leftoverQuantities[i]) + SPACE_SEPARETOR + to_string(leftoverMoney[i]);
        write(writeFd, data.c_str(), data.size());
        close(writeFd);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        logger.logError("Incorrect number of arguments");
        return EXIT_FAILURE;
    }
    
    string storeNameWithFormat = split(argv[1], SLASH)[1];
    string storeName = split(storeNameWithFormat, '.')[0];
    logger.setProccessName("Store " + storeName);

    int readFromMainFd = atoi(argv[2]);
    int writeToMainFd = atoi(argv[3]);

    char buffer_[MAX_BUF];
    int bytesRead = read(readFromMainFd, buffer_, MAX_BUF);
    if (bytesRead <= 0) {
        logger.logError("Failed to read from main process");
        return EXIT_FAILURE;
    }

    buffer_[bytesRead] = '\0';
    vector<string> wantedParts = split(string(buffer_), SPACE_SEPARETOR);
    close(readFromMainFd);

    logger.logInfo("Information received from main process successfully");

    vector<vector<string>> vouchers;
    CSV csv(argv[1]);
    if (csv.readCSV(wantedParts) == -1) {
        logger.logError("Cannot read CSV file");
        return EXIT_FAILURE;
    }
    vouchers = csv.getTable();

    float totalProfit = 0;
    vector<int> leftoverQuantities;
    vector<float> leftoverMoney;

    calculateProfitAndLeftovers(vouchers, wantedParts, totalProfit, leftoverQuantities, leftoverMoney);

    // Send total profit to the main process
    string profit = to_string(totalProfit);
    write(writeToMainFd, profit.c_str(), profit.size());
    close(writeToMainFd);
    logger.logInfo("Profit sent to main process successfully");

    // Send leftover data to other processes if needed
    sendDataToPartsProcesses(storeName,wantedParts, leftoverQuantities, leftoverMoney);
    logger.logInfo("Store process finished successfully");
    exit(EXIT_SUCCESS);
}

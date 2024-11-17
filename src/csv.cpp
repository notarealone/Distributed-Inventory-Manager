#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_set> 

#include "include/csv.hpp"
#include "include/ansi-color.hpp"
#include "include/consts.hpp"

using namespace std;

CSV::CSV(string filePath):filePath(filePath)
{
}

string CSV::trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

int CSV::readCSV() {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << ANSI_RED << "Error: Could not open file " << ANSI_YEL << filePath << ANSI_RST << endl;
        return -1;
    }
    string line;
    while (getline(file, line)) {
        vector<string> row;
        istringstream iss(line);
        string token;
        while (getline(iss, token, ',')) {
            row.push_back(token);
        }
        table.push_back(row);
    }
    file.close();
    return 0;
}


int CSV::readCSV(const vector<string>& itemNames) {
    ifstream file(filePath);
    if (!file.is_open()) 
    {
        cerr << ANSI_RED << "Error: Could not open file " << ANSI_YEL << filePath << ANSI_RST << endl;
        return -1;
    }

    unordered_set<string> itemSet(itemNames.begin(), itemNames.end());
    string line;

    while (getline(file, line)) 
    {
        vector<string> row;
        istringstream iss(line);
        string token;

        while (getline(iss, token, ',')) 
        {
            row.push_back(trim(token));
        }
        if (!row.empty() && itemSet.find(row[0]) != itemSet.end()) 
        {
            table.push_back(row);
        }
    }
    file.close();
    return 0;
}


const CSV::Table& CSV::getTable() const {
    return table;
}

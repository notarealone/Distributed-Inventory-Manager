#ifndef CSV_HPP
#define CSV_HPP

#include <string>
#include <vector>

class CSV {
public:
    using Table = std::vector<std::vector<std::string>>;

    CSV(std::string filePath);

    int readCSV();
    int readCSV(const std::vector<std::string>& itemNames);
    const Table& getTable() const;

private:
    Table table;
    std::string filePath;
    std::string trim(const std::string& str);
};

#endif
#ifndef CSV_HPP
#define CSV_HPP

#include <string>
#include <vector>


class CSV {
public:
    using Table = std::vector<std::vector<std::string>>;

    CSV(std::string filePath);

    int readCSV();
    const Table& getTable() const;

private:
    Table table;
    std::string filePath;
};

#endif
#pragma once
#include <vector>
#include <string>

class CSVReader {
    static std::vector<std::string> readLines(std::string csvFilename);
    static std::vector<std::string> tokenise(std::string csvLine, char separator=',');
};
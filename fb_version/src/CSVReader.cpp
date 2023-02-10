#include "CSVReader.h"
#include <iostream>
#include <fstream>
  
std::vector<std::string> CSVReader::readLines(std::string csvFilename)
{
    std::vector<std::string> lines;
    std::ifstream csvFile{csvFilename};
    std::string line;
    if (csvFile.is_open()){
        while(std::getline(csvFile, line)){
            lines.push_back(line);
        }// end of while
    }
    return lines; 
}
    
std::vector<std::string> CSVReader::tokenise(std::string csvLine, char separator=',')
{
    std::vector<std::string> tokens;
    signed int start, end;
    std::string token;
        start = csvLine.find_first_not_of(separator, 0);
        do{
            end = csvLine.find_first_of(separator, start);
            if (start == csvLine.length() || start == end) break;
            if (end >= 0) token = csvLine.substr(start, end - start);
            else token = csvLine.substr(start, csvLine.length() - start);
            tokens.push_back(token);
        start = end + 1;
        }while(end > 0);

    return tokens; 
}


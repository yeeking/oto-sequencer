#include "ChordUtils.h"

std::map<int,chordSpec> ChordUtils::chordSpecs = {
    {0, {"maj", {0, 4, 7}}}, 
    {1, {"min", {0, 3, 7}}}, 
    {2, {"7", {0, 4, 7, 10}}},  
    {3, {"maj7", {0, 4, 7, 11}}}, 
    {4, {"min7", {0, 3, 7, 10}}}, 
    {5, {"hdim7", {0, 3, 6, 10}}}, 
    {6, {"dim7", {0, 3, 6, 9}}}, 
    {7, {"sus4", {0, 5, 7}}}, 
    {8, {"sus2", {0, 2, 7}}}
};

std::vector<double> ChordUtils::getChord(unsigned int root, unsigned int variation)
{
    //root = root % 12; // weap on 12
    std::vector<double> intervals = ChordUtils::chordSpecs[variation].second;
    for (auto i=0;i<intervals.size(); i++) intervals[i] += root;
    return intervals; 
}



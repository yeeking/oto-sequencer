#include <vector>
#include <map>
#include <string> 

typedef std::pair<std::string,std::vector<double>> chordSpec;

class ChordUtils {
    public:
        /**
         * @brief Returns a vector of notes representing the sent chord
         * 
         * @param root an int normally in the range 0-11 for the chromatic scale index (a->g#)
         * @param variation an which variation? indexes into 
         * @return std::vector<double> a vector of midi notes representing the chord
         */
        static std::vector<double> getChord(unsigned int root, unsigned int variation);
    private: 
        static std::map<int,chordSpec> chordSpecs;
         
};
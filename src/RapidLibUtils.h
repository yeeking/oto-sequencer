#include "../lib/rapidLib.h"
#include <vector>

class NeuralNetwork
{
    public:
    /** generates a regressor to map from x,y inputs in the range
     * 0-1 to midi notes in the range 0-127
    */
    static rapidLib::regression getMelodyStepsRegressor()
    {
        rapidLib::regression network;
        network.setNumHiddenNodes(10);
        std::vector<rapidLib::trainingExample> trainingSet;
        rapidLib::trainingExample  tempExample;
        // generate examples for each corder            
        tempExample.input = { 0, 0};
        tempExample.output = {0, 0, 0, 0, 0, 0, 0, 0 };
        trainingSet.push_back(tempExample); // note this makes a copy
        //
        tempExample.input = { 0, 1};
        tempExample.output = {0, 127, 0, 127, 0, 127, 0, 127 };
        trainingSet.push_back(tempExample); // note this makes a copy
        //
        tempExample.input = { 1, 0};
        tempExample.output = {127, 0, 127, 0, 127, 0, 127, 0 };
        trainingSet.push_back(tempExample); // note this makes a copy
        //  
        tempExample.input = { 1, 1};
        tempExample.output = {127, 127, 127, 127, 127, 127, 127, 127 };
        trainingSet.push_back(tempExample); // note this makes a copy

        network.train(trainingSet);

        return network;
    }

    static void rescale(std::vector<double>& values, double scalar)
    {
        for (int i=0; i < values.size(); ++i)
        {
            values[i] *= scalar;
        }
    }
};
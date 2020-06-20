#include <functional>
#include <iostream>

//#include "../lib/grovepi.h"
//#include "../lib/grove_rgb_lcd.h"
#include "../lib/ml/rapidLib.h"

#include "SimpleClock.h"
#include "Sequencer.h"
//#include "GroveUtils.h"
#include "RapidLibUtils.h"

int main()
{
    Sequencer seqr;
    SequencerEditor seqEditor{&seqr};
    SimpleClock clock{};
    // this will map joystick x,y to 16 sequences
    rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();

    clock.setCallback([&seqr, &seqEditor](){
        seqr.tick();
            // clear the screen in nix
            std::cout << "\x1B[2J\x1B[H";

            std::string disp = SequencerViewer::toTextDisplay(2, 16, &seqr, &seqEditor);
            std::cout << disp << std::endl;
        });

//    clock.start(1000);
    

    
    int x {1};
    while (x != 0)
    {
      std::cin >> x;
      std::cout << x << std::endl;  
    }
}

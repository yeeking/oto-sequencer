#include <functional>
#include <iostream>

#include "../lib/grove/grovepi.h"
#include "../lib/grove/grove_rgb_lcd.h"
#include "../lib/ml/rapidLib.h"

#include "SimpleClock.h"
#include "Sequencer.h"
#include "GroveUtils.h"
#include "RapidLibUtils.h"

int main()
{
    Sequencer seqr;
    SequencerEditor seqEditor{&seqr};
    SimpleClock clock{};
    GrovePi::LCD lcd{};
    // this will map joystick x,y to 16 sequences
    rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();

    clock.setCallback([&seqr, &lcd, &seqEditor](){
        seqr.tick();
            std::string disp = SequencerViewer::toTextDisplay(2, 16, &seqr, &seqEditor);
            std::cout << disp << std::endl;
          //  lcd.setText(disp.c_str());
        });
    try
	{
		// connect to the i2c-line
		lcd.connect();
		// set text and RGB color on the LCD
		lcd.setText("Loading sequencer....");
		lcd.setRGB(0, 255, 0);
    }
    catch(GrovePi::I2CError &error)
	{
		printf(error.detail());
		return -1;
	}

    GroveJoystickXY joy{
        [&network, &seqEditor](float x, float y)
        {
            std::cout << "joy x: " << x << " y: " << y << std::endl;
            std::vector<double> input = {x, y};
            std::vector<double> output = network.run(input);
            // now we have the new sequence... what to 
            // do withg it?
            // pass it into the sequencer of course
            seqEditor.setCurrentSequence(0);
            seqEditor.setCurrentStep(0);
            seqEditor.writeSequenceData(output);    
        }
    };

    //clock.start(1000);
    

/*    
    GroveJoystickDirection joy([&seqEditor, &seqr, &lcd](JoystickEvent event){
        switch (event)
        {
            case JoystickEvent::up:
                //std::cout << "Received jpystick event up" << std::endl;
                seqEditor.moveCursorUp();
                break;
            case JoystickEvent::down:
                //std::cout << "Received jpystick event down" << std::endl;
                seqEditor.moveCursorDown();
                break;
            case JoystickEvent::left:
                //std::cout << "Received jpystick event left" << std::endl;
                seqEditor.moveCursorLeft();              
                break;
            case JoystickEvent::right:
                //std::cout << "Received jpystick event right " << std::endl;
                seqEditor.moveCursorRight();
                break;
            case JoystickEvent::click:
                std::cout << "Received jpystick event click " << std::endl;
                break;   
        }
        // now update the display
        std::string disp = SequencerViewer::toTextDisplay(2, 16, &seqr, &seqEditor);
        std::cout << disp << std::endl;
        lcd.setText(disp.c_str());		
    });
*/
    int x;
    std::cin >> x;
}

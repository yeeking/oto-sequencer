#include <functional>
#include <iostream>
#include <stdio.h>  

#include "../lib/ml/rapidLib.h"

#include "SimpleClock.h"
#include "Sequencer.h"
#include "RapidLibUtils.h"
#include "MidiUtils.h"
#include "GroveUtils.h"
#include "KeyUtils.h"

#include <unistd.h>
#include <termios.h>


// getch with no echo and instant response
// https://stackoverflow.com/a/912796/1240660
char getch() {
  char buf = 0;
  struct termios old = {0};
  if (tcgetattr(0, &old) < 0)
          perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
          perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
          perror ("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
          perror ("tcsetattr ~ICANON");
  return (buf);
}

void redrawConsole(Sequencer& seqr, SequencerEditor& seqEditor)
{ 
   // std::cout << "\x1B[2J\x1B[H";
    std::string disp = SequencerViewer::toTextDisplay(16, 32, &seqr, &seqEditor);
    std::cout << disp << std::endl;
}

void redrawGroveLCD(Sequencer& seqr, SequencerEditor& seqEditor, GrovePi::LCD& lcd)
{ 
    std::string disp = SequencerViewer::toTextDisplay(2, 16, &seqr, &seqEditor);
    std::cout << disp << std::endl;
    lcd.setText(disp.c_str());

}

int main()
{
    KeyReader keyReader;

    const std::map<int, double> key_to_note =
    {
        { 44, 60},
        { 31, 61},
        { 45, 63},
        { 32, 64},
        { 46, 65},
        { 47, 66},
        { 34, 67},
        { 48, 68},
        { 35, 69},
        { 49, 70},
        { 36, 71},
        { 50, 72}
    };


    GrovePi::LCD lcd{};
    
    try
	{
		// connect to the i2c-line
		lcd.connect();
		// set text and RGB color on the LCD
		lcd.setText("Loading sequencer....");
		lcd.setRGB(0, 0, 255);
    }
    catch(GrovePi::I2CError &error)
	{
		printf(error.detail());
		return -1;
	}

    // this constructor will trigger midi initialisation
    MidiStepDataReceiver midiStepReceiver;
    //NaiveStepDataReceiver midiStepReceiver;

    Sequencer seqr{&midiStepReceiver};

    SequencerEditor seqEditor{&seqr};
    SimpleClock clock{};
    // this will map joystick x,y to 16 sequences
    rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();

    clock.setCallback([&seqr, &seqEditor](){
      seqr.tick();
      //redrawConsole(seqr, seqEditor);    
    });

    clock.start(125);
    char input {1};
    
    while (input != 16) // q for quit
    {
        redrawGroveLCD(seqr, seqEditor, lcd);
        input = keyReader.getChar();
        switch(input)
        {
            case 15: // tab 
                seqEditor.cycleEditMode();
                continue;
            case 57: // space
                seqEditor.cycleAtCursor();
                continue;
            case 28: // return
                seqEditor.enterAtCursor();
                continue;
            case 103: // up
                seqEditor.moveCursorUp();
                continue;
            case 105: // left
                seqEditor.moveCursorLeft();
                continue;
            case 106: // right
                seqEditor.moveCursorRight();
                continue;
            case 108: // down
                seqEditor.moveCursorDown();
                continue;     
        }// end switch on key
        // now check all the piano keys
        bool key_note_match{false};
        for (const std::pair<int, double>& key_note : key_to_note)
        {
            if (input == key_note.first) 
            { 
                key_note_match = true;
                seqEditor.enterNoteData(key_note.second);
                break;// break the for loop
            }
        }
    }// end while loop
  return 0;
}

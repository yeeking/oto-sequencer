#include <functional>
#include <iostream>
#include <stdio.h>  

#include "../lib/ml/rapidLib.h"

#include "SimpleClock.h"
#include "Sequencer.h"
#include "RapidLibUtils.h"
#include "MidiUtils.h"
#include "GroveUtils.h"

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
    const std::map<char, double> key_to_note =
    {
        { 'z', 60},
        { 's', 61},
        { 'x', 63},
        { 'd', 64},
        { 'c', 65},
        { 'f', 66},
        { 'v', 67},
        { 'b', 68},
        { 'h', 69},
        { 'n', 70},
        { 'j', 71},
        { 'm', 72}
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
    bool escaped = false;
    
    while (input != 'q')
    {
      if (!escaped) redrawGroveLCD(seqr, seqEditor, lcd);
      input = getch();
      if (!escaped)
      {
        switch(input)
        {
          case '\033': // first escape character cursor key?
            escaped = true;
            continue;
          case '\t': 
            seqEditor.cycleEditMode();
            continue;
          case ' ':
            seqEditor.cycleAtCursor();
            continue;
          case '\n':
            //seqEditor.cycleMode();
            seqEditor.enterAtCursor();
            continue;
          case (wchar_t)(127):
            //seqEditor.deleteAtCursor();
            continue;
        }// send switch on key
        // now check all the piano keys
        bool key_note_match{false};
        for (const std::pair<char, double>& key_note : key_to_note)
        {
          if (input == key_note.first) 
          { 
      //      std::cout << "match" << key_note.second << std::endl;
            key_note_match = true;
            seqEditor.enterNoteData(key_note.second);
            //redraw(seqr, seqEditor);
            break;// break the for loop
          }
        }
        //if (key_note_match) continue; // continue the while loop
      } // end if !escapted
      if (escaped){
        switch(input){
          case '[': 
            continue;
          case 'A':
            // up
            seqEditor.moveCursorUp();
            escaped = false;
            //redraw(seqr, seqEditor);
            continue;
          case 'D':
            // left
            seqEditor.moveCursorLeft();
            escaped = false;
            //redraw(seqr, seqEditor);
            continue;
          case 'C':
            // right
            seqEditor.moveCursorRight();
            escaped = false;
            //redraw(seqr, seqEditor);
            continue;
          case 'B':
            // down
            seqEditor.moveCursorDown();
            escaped = false;
            //redraw(seqr, seqEditor);
            continue;     
        }//  end switch on scapted mode
      }// end if escapted mode
      //redrawConsole(seqr, seqEditor);    

    }// end while loop
  return 0;
}

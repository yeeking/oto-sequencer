#include <functional>
#include <iostream>
#include <stdio.h>  
#include <fstream>
#include <string>

#include "../lib/ml/rapidLib.h"

#include "SimpleClock.h"
#include "Sequencer.h"
#include "SequencerUtils.h"
#include "RapidLibUtils.h"
#include "MidiUtils.h"

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

/** attempts to get the serial device filename 
 * if it fails, returns ""
*/
std::string getSerialDevice(std::string devPrefix = "/dev/ttyACM", bool debug = true)
{
    bool open = false;
    for (auto i=0;i<10;i++)
    {
      ofstream serial_bus;
      std::string port = devPrefix + std::to_string(i);
      if (debug)
        std::cout << "main.cpp::getSerialDevice trying to open " << port << std::endl;
      try{
        serial_bus.open (port);
        if (serial_bus.is_open()){
          serial_bus.close();
          // got one
          if (debug)
            std::cout << "main.cpp::getSerialDevice opened " << port << std::endl;
          return port; 
          break;
        }
        else {
          if (debug)
            std::cout << "main.cpp::getSerialDevice could not open " << port << std::endl;
        }
      }
      catch(int exception){
        if (debug)
          std::cout << "main.cpp::getSerialDevice open " << port << " failed " << std::endl;
      }
    }
    // fail condition
    return "";
}

void redrawToConsole(const std::string& output)
{ 
  std::cout << "\x1B[2J\x1B[H";
  std::cout << output << std::endl;
}

void redrawToWio(const std::string& device, const std::string& output)
{    
    ofstream serial_bus;
    serial_bus.open (device);
    serial_bus << output << "\t"; // last character triggers the redraw
    serial_bus.close();
}

int main()
{
    std::string serialDev = getSerialDevice();
    const std::map<char, double> key_to_note =
    {
      { 'z', 48},
      { 's', 49},
      { 'x', 50},
      { 'd', 51},
      { 'c', 52},
      { 'f', 53},
      { 'v', 54},
      { 'b', 55},
      { 'h', 56},
      { 'n', 57},
      { 'j', 59},
      { 'm', 60}
    };
    std::map<char, double>::iterator it;
    
    MidiUtils midiUtils;
    midiUtils.interactiveInitMidi();
    midiUtils.allNotesOff();
  
    SimpleClock clock{};

    //NaiveStepDataReceiver midiStepReceiver;
    Sequencer seqr{16, 8};
    SequencerEditor seqEditor{&seqr};
   
    // set up a midi note triggering callback 
    // on all steps
    seqr.setAllCallbacks(
        [&midiUtils, &clock](std::vector<double>* data){
          if (data->size() >= 3)
          {
            double channel = data->at(Step::channelInd);
            double offTick = clock.getCurrentTick() + data->at(Step::lengthInd);
            // make the length quantised by steps
            double noteVolocity = data->at(Step::velInd);
            double noteOne = data->at(Step::note1Ind);
            midiUtils.playSingleNote(channel, noteOne, noteVolocity, offTick);            
          }
        }
    );

  // tick the sequencer and send any queued notes
    clock.setCallback([&seqr, &seqEditor, &midiUtils, &clock, &serialDev](){
      //std::cout << "main.cpp clock callback " << clock.getCurrentTick() << std::endl;
      midiUtils.sendQueuedMessages(clock.getCurrentTick());
      seqr.tick();
      std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &seqEditor);
      redrawToConsole(output);
      if (serialDev != "")
        redrawToWio(serialDev, output);    
    });

    // this will map joystick x,y to 16 sequences
    rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();
    int clockIntervalMs = 125;  
    clock.start(clockIntervalMs);
    char input {1};
    bool escaped = false;
    bool redraw = false; 
    while (input != 'q')
    {
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
          case '-':
            clockIntervalMs += 5;
            clock.stop();
            clock.start(clockIntervalMs);
            continue;
          case '=':
            clockIntervalMs -= 5;
            clock.stop();
            clock.start(clockIntervalMs);
            continue;
          case '\n':
            //seqEditor.cycleMode();
            seqEditor.enterAtCursor();
            continue;
          case (wchar_t)(127):
            seqEditor.enterNoteData(0);
            continue;
        }// send switch on key
        // now check all the piano keys
        bool key_note_match{false};
        for (const std::pair<char, double>& key_note : key_to_note)
        {
          if (input == key_note.first) 
          { 
           // std::cout << "match" << key_note.second << std::endl;
            key_note_match = true;
            seqEditor.enterNoteData(key_note.second); 
            redraw = true;   
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
            redraw = true;   
            continue;
          case 'D':
            // left
            seqEditor.moveCursorLeft();
            escaped = false;
            redraw = true;   
            continue;
          case 'C':
            // right
            seqEditor.moveCursorRight();
            escaped = false;
            redraw = true;   
            continue;
          case 'B':
            // down
            seqEditor.moveCursorDown();
            escaped = false;
            redraw = true;   
            redraw = true;   
        }
      }
      if (redraw)
      {
        std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &seqEditor);
        redrawToConsole(output);
        if (serialDev != "")
          redrawToWio(serialDev, output);
      }
    }// end of key input loop
  clock.stop();

  midiUtils.allNotesOff();
  return 0;
}



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
#include "IOUtils.h"

int main()
{
  // wio terminal serial display device if available
    std::string wioSerial = Display::getSerialDevice();
  // maps computer keyboard to midi notes
    std::map<char, double> key_to_note = MidiUtils::getKeyboardToMidiNotes();
    
    MidiUtils midiUtils;
    midiUtils.interactiveInitMidi();
    midiUtils.allNotesOff();
  
    SimpleClock clock{};

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
    clock.setCallback([&seqr, &seqEditor, &midiUtils, &clock, &wioSerial](){
      //std::cout << "main.cpp clock callback " << clock.getCurrentTick() << std::endl;
      midiUtils.sendQueuedMessages(clock.getCurrentTick());
      seqr.tick();
      std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &seqEditor);
      Display::redrawToConsole(output);
      if (wioSerial != "")
        Display::redrawToWio(wioSerial, output);    
    });

    // this will map joystick x,y to 16 sequences
    //rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();
    int clockIntervalMs = 125;  
    clock.start(clockIntervalMs);
    char input {1};
    bool escaped = false;
    bool redraw = false; 

    while (input != 'q')
    {
      input = KeyReader::getCharNoEcho();
      if (!escaped)
      {
        switch(input)
        {
          case '\033': // first escape character cursor key?
            escaped = true;
            continue;
          case '\t':  // next 'mode'
            seqEditor.cycleEditMode();
            continue;
          case ' ': // mute
            seqEditor.cycleAtCursor();
            continue;
          case '-': // slower
            clockIntervalMs += 5;
            clock.stop();
            clock.start(clockIntervalMs);
            continue;
          case '=': // faster
            clockIntervalMs -= 5;
            clock.stop();
            clock.start(clockIntervalMs);
            continue;
          case '\n': // enter
            //seqEditor.cycleMode();
            seqEditor.enterAtCursor();
            continue;
          case (wchar_t)(127): // delete
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
        }
      }
      if (redraw)
      {
        std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &seqEditor);
        Display::redrawToConsole(output);
        if (wioSerial != "")
          Display::redrawToWio(wioSerial, output);
      }
    }// end of key input loop
  clock.stop();

  midiUtils.allNotesOff();
  return 0;
}



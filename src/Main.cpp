#include <functional>
#include <iostream>
#include <stdio.h>  
#include <fstream>
#include <string>
#include <assert.h>
//#include "../lib/ml/rapidLib.h"

#include "SimpleClock.h"
#include "Sequencer.h"
#include "SequencerUtils.h"
#include "RapidLibUtils.h"
#include "MidiUtils.h"
#include "IOUtils.h"

void updateClockCallback(SimpleClock& clock,
			std::vector<Sequencer*>& seqrs,
                    Sequencer* currentSeqr, 
                    SequencerEditor& seqEditor, 
                    MidiUtils& midiUtils, 
                    std::string& wioSerial)
{
  clock.setCallback([&seqrs, currentSeqr, &seqEditor, &midiUtils, &clock, &wioSerial](){
      midiUtils.sendQueuedMessages(clock.getCurrentTick());

      std::string output = SequencerViewer::toTextDisplay(9, 13, currentSeqr, &seqEditor);
      Display::redrawToConsole(output);
      if (wioSerial != "")
        Display::redrawToWio(wioSerial, output);
      for (Sequencer* s : seqrs)
	{
	  //s->tick(false);
	  // only do a proper trigger on current seq
	  if (currentSeqr == s) s->tick();
	  // otherwise tick without trigger
	  else s->tick(false);
	}

    });
}

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

    // create a vector of sequences
    std::vector<Sequencer*> seqrs{};
    for (int i=0;i<4;i++) seqrs.push_back(new Sequencer{16,8});
    Sequencer* currentSeqr = seqrs[0];
    SequencerEditor seqEditor{currentSeqr};
   
    for (Sequencer* seqr : seqrs)
    {
      // set up a midi note triggering callback 
      // on all steps
      seqr->setAllCallbacks(
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
    }

    updateClockCallback(clock,
			seqrs, 
                        currentSeqr, 
                        seqEditor, 
                        midiUtils, 
                        wioSerial);
    
    // this will map joystick x,y to 16 sequences
    //rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();
    int clockIntervalMs = 125;  
    clock.start(clockIntervalMs);
    char input {1};
    bool escaped = false;
    bool redraw = false; 
    bool running = true; 

    while (input != 'q')
    {
      // check if we have make a sequencer config edit 
      if (seqEditor.getEditMode() == SequencerEditorMode::configuringSequence)
      {
        for (Sequencer* seqr : seqrs) {
          if (seqr != currentSeqr)
            seqr->copyChannelAndTypeSettings(currentSeqr);
        }
      }
      input = KeyReader::getCharNoEcho();
      //std::cout << "got input:: " << ((int)input) << std::endl;
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
          case 'p': // stop / start
            midiUtils.allNotesOff();
            if (running) clock.stop();
            else clock.start(clockIntervalMs);
            running = !running; 
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
          case 'r':
            //seqEditor
            seqEditor.resetAtCursor();
            continue;
        //  case (wchar_t)(127): // delete
        //    seqEditor.enterNoteData(0);
        //    continue;
        }// send switch on key
        // reset the step under the cursor
        if ((int)input == 127) 
        {
          
        }
        // this is the implementation
        // of sequencer switching where keys 1->seqrs.size
        // trigger switch to different sequencer
        for (int i=0;i<seqrs.size();i++)
        {
	  if (input == 49 + i) // ascii 1 == 49
          //if (false)
          {
            assert (i < seqrs.size());
            midiUtils.allNotesOff();
            currentSeqr = seqrs[i];
            //clock needs to know it is calling 
            //tick on a different sequencer
            updateClockCallback(clock,
	    			seqrs, 
                    currentSeqr, 
                    seqEditor, 
                    midiUtils, 
                    wioSerial);
            seqEditor.setSequencer(currentSeqr);
            seqEditor.resetCursor();
          }
        }
        // this is the implementation 
        // of note entry, where bottom two rows
        // of keyboard map to piano keys
        bool key_note_match{false};
        for (const std::pair<char, double>& key_note : key_to_note)
        {
          if (input == key_note.first) 
          { 
            key_note_match = true;
            seqEditor.enterNoteData(key_note.second); 
            redraw = true;   
            break;// break the for loop
          }
        }
        //if (key_note_match) continue; // continue the while loop
      } // end if !escapted
      // now the escaped keys - allows cursor keys to be used
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
            continue; 
        }
      }
      if (redraw)
      {
        std::string output = SequencerViewer::toTextDisplay(9, 13, currentSeqr, &seqEditor);
        Display::redrawToConsole(output);
        if (wioSerial != "") Display::redrawToWio(wioSerial, output);
      }
      // make sure all sequences have the same channels and types as eachother
   
    }// end of key input loop
  clock.stop();

  midiUtils.allNotesOff();
  for (Sequencer* s : seqrs) delete s;
  return 0;
}



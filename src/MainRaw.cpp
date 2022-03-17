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


void updateClockCallbackIndex(SimpleClock& clock,
			              std::vector<Sequencer*>& seqrs,
                    int seqInd, 
                    SequencerEditor& seqEditor, 
                    MidiUtils& midiUtils, 
                    std::string& wioSerial)
{
clock.setCallback([&seqrs, seqInd, &seqEditor, &midiUtils, &clock, &wioSerial](){
      midiUtils.sendQueuedMessages(clock.getCurrentTick());

      std::string output = SequencerViewer::toTextDisplay(9, 13, seqrs[seqInd], &seqEditor);
      if (wioSerial != "")
        Display::redrawToWio(wioSerial, output);
      else
        Display::redrawToConsole(output);

      for (int i=0;i<seqrs.size(); ++i)
      {
        if (i == seqInd) seqrs[i]->tick();
        else seqrs[i]->tick(false);   
      }
    });
}



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
      if (wioSerial != "")
        Display::redrawToWio(wioSerial, output);
      else
        Display::redrawToConsole(output);
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

void setupMidiViaWio( MidiUtils& midiUtils, std::string wioSerial, KeyReader& keyReader)
{
    int midiDev = -1;
    std::vector<std::string> midiOuts = midiUtils.getOutputDeviceList();
    while(midiDev == -1)
    {
        std::cout << "Attempting device selection " << std::endl;
        int option_ind = 0;
        std::string msg {};
        for (std::string& dev : midiOuts)
        {
            option_ind ++;
            dev = std::to_string(option_ind) + ":" + dev;
            msg += dev + "\n";
        }
        msg += "Choose 1 to " + std::to_string(midiOuts.size()) + "\n";        
        std::cout << msg << std::endl;
    	    Display::redrawToWio(wioSerial, msg);
        //lcd.setText(msg.c_str());
        midiDev = keyReader.getChar() - 2;
        std::cout << "You chose " << midiDev << std::endl;
        if (midiDev > midiOuts.size() || midiDev < 0) midiDev = -1;
    }
    std::cout << "selecting a device " << midiDev << std::endl;
    midiUtils.selectOutputDevice(midiDev);
    midiUtils.allNotesOff();
}


int main()
{
  // wio terminal serial display device if available
    std::string wioSerial = Display::getSerialDevice();
    KeyReader keyReader{"/dev/input/event0"};

  // maps computer keyboard to midi notes
    std::map<char, double> key_to_note = MidiUtils::getKeyboardToMidiNotes();
    
    MidiUtils midiUtils;
    if (wioSerial != "") setupMidiViaWio(midiUtils, wioSerial, keyReader);
    else midiUtils.interactiveInitMidi();
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

    // updateClockCallback(clock,
		//             	seqrs, 
    //                     currentSeqr, 
    //                     seqEditor, 
    //                     midiUtils, 
    //                     wioSerial);
        updateClockCallbackIndex(clock,
		            	          seqrs, 
                        0, 
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

    std::map<int,char> keyMap = KeyReader::getCharNameMap();
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
      //input = KeyReader::getCharNoEcho();
      //input = keyMap[(int)keyReader.getChar()];
        input = keyReader.getChar();
        int asciiInput = (int) keyMap[(int)input];
        char letterInput = keyMap[(int) input ];
    switch(letterInput)
    {
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
        case 'U':
        // up
        seqEditor.moveCursorUp();
        escaped = false;
        redraw = true;   
        continue;
        case 'L':
        // left
        seqEditor.moveCursorLeft();
        escaped = false;
        redraw = true;   
        continue;
        case 'R':
        // right
        seqEditor.moveCursorRight();
        escaped = false;
        redraw = true;   
        continue;
        case 'D':
        // down
        seqEditor.moveCursorDown();
        escaped = false;
        redraw = true;  
        continue; 

    }// send switch on key

    // this is the implementation
    // of sequencer switching where keys 1->seqrs.size
    // trigger switch to different sequencer
    for (int i=0;i<seqrs.size();i++)
    {
        //if (false)
        if (asciiInput == 49 + i) // ascii 1 == 49
        {
            assert (i < seqrs.size());
            midiUtils.allNotesOff();
            currentSeqr = seqrs[i];
            //clock needs to know it is calling 
            //tick on a different sequencer
            // updateClockCallback(clock,
            //         seqrs, 
            //         currentSeqr, 
            //         seqEditor, 
            //         midiUtils, 
            //         wioSerial);
              updateClockCallbackIndex(clock,
                    seqrs, 
                    i, 
                    seqEditor, 
                    midiUtils, 
                    wioSerial);

            seqEditor.setSequencer(seqrs[i]);
            seqEditor.resetCursor();
            break;
        }
    }
    // this is the implementation 
    // of note entry, where bottom two rows
    // of keyboard map to piano keys
    bool key_note_match{false};
    for (const std::pair<char, double>& key_note : key_to_note)
    {
        if (letterInput == key_note.first) 
        { 
        key_note_match = true;
        seqEditor.enterNoteData(key_note.second); 
        redraw = true;   
        break;// break the for loop
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



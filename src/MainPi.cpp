#include <functional>
#include <iostream>
#include <fstream>
#include <stdio.h>  

#include "../lib/ml/rapidLib.h"

#include "SimpleClock.h"
#include "Sequencer.h"
#include "SequencerUtils.h"
#include "RapidLibUtils.h"
#include "MidiUtils.h"
#include "GroveUtils.h"
#include "IOUtils.h"
#include <unistd.h>
#include <termios.h>

void redrawGroveLCD(Sequencer& seqr, SequencerEditor& seqEditor, GrovePi::LCD& lcd)
{ 
    std::string disp = SequencerViewer::toTextDisplay(2, 16, &seqr, &seqEditor);
    std::cout << disp << std::endl;
    lcd.setText(disp.c_str());
}

void updateLCDColour(SequencerEditor& editor, GrovePi::LCD& lcd)
{
    switch(editor.getEditMode() )
    {
        case SequencerEditorMode::editingStep:
            lcd.setRGB(50, 0, 50);
            break;
        case SequencerEditorMode::selectingSeqAndStep:
            lcd.setRGB(0, 0, 50);
            break;
        case SequencerEditorMode::settingSeqLength:
            lcd.setRGB(0, 50, 0);
            break;
    }
}

void setupMidiViaLCD(MidiUtils& midiUtils, KeyReader& keyReader, GrovePi::LCD& lcd)
{
    int midiDev = -1;
    std::vector<std::string> midiOuts = midiUtils.getOutputDeviceList();
    while(midiDev == -1)
    {
        std::cout << "Attempting device selection " << std::endl;
        int option_ind = 0;
        for (std::string& dev : midiOuts)
        {
            option_ind ++;
            dev = std::to_string(option_ind) + ":" + dev;
            std::cout << dev << std::endl;
            lcd.setText(dev.c_str());
            sleep(1);
        }
        std::string msg = "Choose 1 to " + std::to_string(midiOuts.size());
        
        std::cout << msg << std::endl;
        lcd.setText(msg.c_str());
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
    KeyReader keyReader;
    // maps from raw event input keycodes
    // to midi notes
    const std::map<char, double> key_to_note = MidiUtils::getKeyboardToMidiNotes();

    // access to the wio
    std::string wioSerial = Display::getSerialDevice();
    
    // access to the rgb lcd
    GrovePi::LCD lcd{};
    bool useLCD = false;
    try
	{
		// connect to the i2c-line
		lcd.connect();
		// set text and RGB color on the LCD
		lcd.setText("Loading sequencer....");
		lcd.setRGB(255, 0, 0);
        useLCD = true; 
    }
    catch(GrovePi::I2CError &error)
	{
		printf(error.detail());
		//return -1;
	}

    // this constructor will trigger midi initialisation
    MidiUtils midiUtils;
    //midiUtils.interactiveInitMidi();
    //midiUtils.allNotesOff();
    setupMidiViaLCD(midiUtils, keyReader, lcd);
    Sequencer seqr{16, 16};
    SequencerEditor seqEditor{&seqr};
    SimpleClock clock{};
    // this will map joystick x,y to 16 sequences
    //rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();

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

    clock.setCallback([&seqr, &seqEditor, &midiUtils, &clock, &wioSerial](){
      midiUtils.sendQueuedMessages(clock.getCurrentTick());
      seqr.tick();
  
      if (wioSerial != "")
      {
        std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &seqEditor);
        Display::redrawToWio(wioSerial, output);    
      }
      else 
      {
        std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &seqEditor);
        Display::redrawToConsole(output);
      }
    });

    clock.start(125);
    char input {1};
    
    while (input != 16) // q for quit
    {
        if (useLCD) redrawGroveLCD(seqr, seqEditor, lcd);
        if (wioSerial != "")
        {
            std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &seqEditor);
            Display::redrawToWio(wioSerial, output);    
        }
        else 
        {
            std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &seqEditor);
            Display::redrawToConsole(output);
        }
        input = keyReader.getChar();
        switch(input)
        {
            case 15: // tab 
                seqEditor.cycleEditMode();
                updateLCDColour(seqEditor, lcd);
                continue;
            case 57: // space
                seqEditor.cycleAtCursor();
                continue;
            case 28: // return
                seqEditor.enterAtCursor();
                updateLCDColour(seqEditor, lcd);
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
    midiUtils.allNotesOff();
  return 0;
}

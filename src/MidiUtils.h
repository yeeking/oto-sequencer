#pragma once

#include "Sequencer.h"

#include "/usr/include/rtmidi/RtMidi.h"


/** implementation of the STepDataReceiver abstract class 
 * which can send midi data when a step is triggered
*/
class MidiStepDataReceiver  
{
  public:
    MidiStepDataReceiver() 
    {
      try {
        midiout = new RtMidiOut();
      }
      catch ( RtMidiError &error ) {
        std::cout << "MidiStepDataReceiver::initMidi problem creating RTNidiOut. Error message: " << error.getMessage() << std::endl; 

      }  
    }
    ~MidiStepDataReceiver()
    {
      delete midiout;
    }

  void interactiveInitMidi()
    {
   
      std::string portName;
      unsigned int i = 0, nPorts = midiout->getPortCount();
      if ( nPorts == 0 ) {
        std::cout << "No output ports available!" << std::endl;
      }

      if ( nPorts == 1 ) {
        std::cout << "\nOpening " << midiout->getPortName() << std::endl;
      }
      else {
        for ( i=0; i<nPorts; i++ ) {
          portName = midiout->getPortName(i);
          std::cout << "  Output port #" << i << ": " << portName << '\n';
        }

        do {
          std::cout << "\nChoose a port number: ";
          std::cin >> i;
        } while ( i >= nPorts );
      }

      std::cout << "\n";
      std::cout << "Preparing to open the port... " << std::endl;
      midiout->openPort( i );
      std::cout << "Port opened... " << std::endl;
      

    }


    std::vector<std::string> getOutputDeviceList()
    {
      std::vector<std::string> deviceList;

      std::string portName;
      unsigned int i = 0, nPorts = midiout->getPortCount();
      // if ( nPorts == 0 ) {
      //   std::cout << "No output ports available!" << std::endl;
      // }

      // if ( nPorts == 1 ) {
      //   std::cout << "\nOpening " << midiout->getPortName() << std::endl;
      // }
      //else {
        for ( i=0; i<nPorts; i++ ) {
          deviceList.push_back(midiout->getPortName(i));
        }
      //}
      return deviceList;
    }
    void selectOutputDevice(int deviceId)
    {
      //std::cout << "\n";
      //std::cout << "Preparing to open the port... " << std::endl;
      midiout->openPort( deviceId );
      //std::cout << "Port opened... " << std::endl;
    }


    void playSingleNote(int channel, int note, int velocity, double lengthMs)
    {
      //std::cout << "MidiStepDataReceiver:: playSingleNote "<< std::endl;
      std::vector<unsigned char> message = {0, 0, 0};

      message[0] = 144 + channel; // 128 + channel
      message[1] = note; // note value
      message[2] = velocity; // velocity value
      midiout->sendMessage( &message );
      queueNoteOff(channel, note, lengthMs);
    }


  private:

     void queueNoteOff(int channel, int note, double lengthMs)
    {
      std::vector<unsigned char> message = {0, 0, 0};
      message[0] = 128 + channel;
      message[1] = note;
      message[2] = 0;
      midiout->sendMessage( &message );
    }

    /** stores the midi out port */
    RtMidiOut *midiout;
};


class MidiQueue {
  public:
   MidiQueue()
   {

   }

   
   private:

};

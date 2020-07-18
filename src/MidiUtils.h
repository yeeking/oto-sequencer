#pragma once

#include "Sequencer.h"
#include "EventQueue.h"

#include "/usr/include/rtmidi/RtMidi.h"


/**
 * 
*/
class MidiUtils 
{
  public:
    MidiUtils() 
    {
      try {
        midiout = new RtMidiOut();
      }
      catch ( RtMidiError &error ) {
        std::cout << "MidiStepDataReceiver::initMidi problem creating RTNidiOut. Error message: " << error.getMessage() << std::endl; 

      }  
    }
    ~MidiUtils()
    {
      delete midiout;
    }

/** 
 * Presents command line promots to the user to allow them to initiate MIDI output
*/
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


/** returns a list of midi output devices */
    std::vector<std::string> getOutputDeviceList()
    {
      std::vector<std::string> deviceList;

      std::string portName;
      unsigned int i = 0, nPorts = midiout->getPortCount();
        for ( i=0; i<nPorts; i++ ) {
          deviceList.push_back(midiout->getPortName(i));
        }
      return deviceList;
    }
    /** opens the sent output device, ready for use */
    void selectOutputDevice(int deviceId)
    {
      midiout->openPort( deviceId );
    }

  /** play a note */
    void playSingleNote(int channel, int note, int velocity, long offTick)
    {
      //std::cout << "MidiStepDataReceiver:: playSingleNote "<< std::endl;
      std::vector<unsigned char> message = {0, 0, 0};

      message[0] = 144 + channel; // 128 + channel
      message[1] = note; // note value
      message[2] = velocity; // velocity value
     // std::cout << "playSingleNote " << message[1] << "off "<< offTick << std::endl;

      midiout->sendMessage( &message );
      queueNoteOff(channel, note, offTick);
    }
    /** send any messages that are due to be sent at the sent tick
     * generally this means note offs.
    */
    void sendQueuedMessages(long tick)
    {
      eventQ.triggerAndClearEventsAtTimestamp(tick);
    }


  private:
    /** stores the midi out port */
    RtMidiOut *midiout;
    /** stores the queue of events*/
    EventQueue eventQ;

     void queueNoteOff(int channel, int note, long offTick)
    { 
      std::vector<unsigned char> message = {0, 0, 0};
      message[0] = 128 + channel;
      message[1] = note;
      message[2] = 0;
      RtMidiOut *midioutP = midiout;
      midioutP->sendMessage( &message ); 
       
      // have to send in a new pointer to midiout
      // by value as otherwise bad things happen.
      // same for message because it gets destructed 
      // otherwise
      //https://en.cppreference.com/w/cpp/language/lambda
      eventQ.addEvent(offTick, [&message, midioutP](){
      //eventQ.addEvent(offTick, [=](){
        std::cout << "EventQqueueNoteOff " << message[1] << std::endl;
        // we have to make a copy of the message
        // as otherwise the origina message appears
        // as const and sendMessage won't accept const.
          std::vector<unsigned char> msg2 = {0, 0, 0};
          msg2[0] = message[0];
          msg2[1] = message[1];
          msg2[2] = message[2];
            midioutP->sendMessage( &msg2 ); 
        });
    }
};



#pragma once

#include <list>
#include "/usr/include/rtmidi/RtMidi.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
 

typedef std::vector<unsigned char> MidiMessage;
typedef std::vector<MidiMessage> MidiMessageVector;

struct TimeStampedMessages{
    long timestamp;
    MidiMessageVector messages;
};
/** 
 * Maintains a linked list of midi messages tagged with timestamps
*/
class MidiQueue
{
    public:
        MidiQueue();
        /** q a message at the specified time point*/
        void addMessage(const long timestamp, const MidiMessage& msg);
        /** get all q's messages for the specified time point and remove them from the linked likst */
        MidiMessageVector getAndClearMessages(long timestamp);
    private:    
        std::list<TimeStampedMessages> messageList;
};

MidiQueue::MidiQueue()
{

}
void MidiQueue::addMessage(const long timestamp, const MidiMessage& msg)
{
   // iterate over the list and insert the event 
    // at a point where the ts ==  or < 
    bool done = false;
    for ( TimeStampedMessages& item : messageList)
    {
        if (item.timestamp == timestamp)
        {
            item.messages.push_back(msg);// now we pass by value
            done = true;
            break;
        } 
    }
    if (!done)
    {
       // std::cout << "EventQueue::addEvent no timestapm match for " << timestamp << std::endl;        
        TimeStampedMessages item {timestamp, MidiMessageVector{msg}};
        messageList.push_back(item);
    }
}
MidiMessageVector MidiQueue::getAndClearMessages(long timestamp)
{
    MidiMessageVector retMessages{};
    std::list<TimeStampedMessages>::iterator it = messageList.begin();
    while(it!=messageList.end())
    //for (it=messageList.begin(); it!=messageList.end(); ++it)
    {            
        //TimestampedCallbacks item = *it;
        if (it->timestamp == timestamp)
        //if (it->timestamp == timestamp) 
        {
            // trigger all the callbacks 
            for (MidiMessage& msg : it->messages)
            {
                retMessages.push_back(msg);            
            }
            // erase it
            it = messageList.erase(it);
           // break;
        }
        else ++it;
    }
    return retMessages;
}

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

    static std::map<int,char> getIntToDrumMap()
    {
    std::map<int, char> intToDrum = 
      {
        {0, 'B'}, 
        {1, 's'},
        {2, 'S'},
        {3, 'r'},
        {4, 'H'},
        {5, 'h'},
        {6, 't'},
        {7, 'T'},
        {8, 'c'},
        {9, 'R'},
        {10, 'C'},
        {11, 'p'}    
      };
      return intToDrum;
    }

    static std::map<char,int> getDrumToMidiNoteMap()
    {
     std::map<char,int> drumToInt = 
      {
        {'B', 36}, 
        {'s', 38},
        {'S', 40},
        {'r', 37},
        {'H', 42},
        {'h', 46},
        {'t', 50},
        {'T', 45},
        {'c', 39},
        {'R', 51},
        {'C', 57},
        {'p', 75}    
      };
      return drumToInt;
    }

    static std::map<char, double> getKeyboardToMidiNotes(int transpose = 0)
    {
      std::map<char, double> key_to_note =
      {
        { 'z', 48+transpose},
        { 's', 49+transpose},
        { 'x', 50+transpose},
        { 'd', 51+transpose},
        { 'c', 52+transpose},
        { 'v', 53+transpose},
        { 'g', 54+transpose},
        { 'b', 55+transpose},
        { 'h', 56+transpose},
        { 'n', 57+transpose},
        { 'j', 58+transpose},
        { 'm', 59+transpose}
      };
      return key_to_note;
      
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
    /** opens the sent output device, ready for use 
     * Should be in the range 0->(number of ports-1) inclusive
    */
    void selectOutputDevice(int deviceId)
    {
      midiout->openPort( deviceId );
    }
    /** send note off messages on all channels to all notes */
    void allNotesOff()
    {
      std::cout << "MidiUtils:: All notes off " << std::endl;
      std::vector<unsigned char> message = {0, 0, 0};

      for (char channel = 0; channel < 16; ++channel)
      {
        std::cout << "Notes off on channel: " << std::to_string(channel) << std::endl;
        for (char note = 0; note < 127; ++note)
        {
          message[0] = 128 + channel;
          message[1] = note;
          message[2] = 0;
          std::this_thread::sleep_for (std::chrono::milliseconds(1));
          midiout->sendMessage( &message );
        }
      }
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
      int msgind = 0;
      for (MidiMessage& msg : midiQ.getAndClearMessages(tick))
      {
        midiout->sendMessage(&msg);
        msgind ++;
      }
    }
    /** stores the midi out port */
    RtMidiOut *midiout;


  private:
    /** stores the queue of events*/
    //EventQueue eventQ;
    MidiQueue midiQ;
    
    void queueNoteOff(int channel, int note, long offTick)
    {
      std::vector<unsigned char> message = {0, 0, 0};
      message[0] = 128 + channel;
      message[1] = note;
      message[2] = 0;
      midiQ.addMessage(offTick, message);
    }
};



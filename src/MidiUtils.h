#pragma once

#include <list>
#include "/usr/include/rtmidi/RtMidi.h"


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

    std::list<TimeStampedMessages>::iterator it;
    for (it=messageList.begin(); it!=messageList.end(); ++it)
    {
        //TimestampedCallbacks item = *it;
        if (it->timestamp == timestamp)
        //if (it->timestamp == timestamp) 
        {
            // trigger all the callbacks 
            std::cout << "MidiQueue::getAndClearMessages : " << it->messages.size() << std::endl;
            for (MidiMessage& msg : it->messages)
            {
                retMessages.push_back(msg);            
            }
            // erase it
            messageList.erase(it);
            // go back one so we can process the next one
            it--;
           // break;
        }
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
      for (MidiMessage& msg : midiQ.getAndClearMessages(tick))
      {
        midiout->sendMessage(&msg);
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



#include "MidiUtils.h"

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


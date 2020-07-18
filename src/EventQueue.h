#include <functional>
#include <vector>
#include <list>

#pragma once

typedef std::function<void()> SequencerCallback;
typedef std::vector<SequencerCallback> CallbackVector;

/** models a vedtor of functions mapped to a timestamp*/
struct TimestampedCallbacks{
    long timestamp;
    CallbackVector callbacks;
};


/**
 * Class used to maintain a sorted list of timestamped events
 * The idea is that c clock function triggers the calling of these evens
 * by sending in the current time
 */

class EventQueue
{
    public:
        EventQueue();
        /** add an event to the queue with the specified timestamp */
        void addEvent(long timestamp, SequencerCallback callback);
        /** trigger events at the sent timestamp */
        void triggerAndClearEventsAtTimestamp(long timestamp);
        /** trigger events now - it will internally work out what the current time is */
        void triggerAndClearEventsAtNow();
        /** returns the vector of callbacks attached to the sent timestamp */
        CallbackVector getEventsAtTimestamp(long timestamp);        
    private:
        std::list<TimestampedCallbacks> timesAndCallbacks;
        bool timestampCloseEnough(long ts1, long ts2);
};


bool EventQueue::timestampCloseEnough(long ts1, long ts2)
{
//     long diff = ts1 - ts2;
//     diff = diff * diff;
//    if (diff < 25) return true; // max 5 ms
   if (ts1 == ts2) return true;
    return false;
}


EventQueue::EventQueue()
{

}

void EventQueue::addEvent(long timestamp, SequencerCallback callback)
{
    // iterate over the list and insert the event 
    // at a point where the ts ==  or < 
    bool done = false;
    for ( TimestampedCallbacks & item : timesAndCallbacks)
    {
        if (timestampCloseEnough(item.timestamp, timestamp))
            //item.timestamp == timestamp) // insert it here
        {
            //std::cout << "EventQueue::addEvent ts match. inserting here. "<< std::endl;
            item.callbacks.push_back(callback);
            //std::cout << "EventQueue::addEvent ts match. inserting here. new len "<< item.callbacks.size() << std::endl;       
            done = true;
            break;
        } 
    }
    if (!done)
    {
       // std::cout << "EventQueue::addEvent no timestapm match for " << timestamp << std::endl;        
        TimestampedCallbacks item {timestamp, CallbackVector{callback}};
        timesAndCallbacks.push_back(item);
    }
}
/** trigger events at the sent timestamp */
void EventQueue::triggerAndClearEventsAtTimestamp(long timestamp)
{
    std::list<TimestampedCallbacks>::iterator it;
    for (it=timesAndCallbacks.begin(); it!=timesAndCallbacks.end(); ++it)
    {
        //TimestampedCallbacks item = *it;
        if (timestampCloseEnough(it->timestamp, timestamp))
        //if (it->timestamp == timestamp) 
        {
            // trigger all the callbacks 
            std::cout << "EventQueue::triggerAndClearEventsAtTimestamp callbacks : " << it->callbacks.size() << std::endl;
            for (SequencerCallback& callback : it->callbacks)
            {
                callback();            
            }
            // erase it
            timesAndCallbacks.erase(it);
            // go back one so we can process the next one
            it--;
           // break;
        }
    }  
}
/** trigger events now - it will internally work out what the current time is */
void EventQueue::triggerAndClearEventsAtNow()
{

}   
/** returns the vector of callbacks attached to the sent timestamp */
CallbackVector EventQueue::getEventsAtTimestamp(long timestamp)
{
    CallbackVector callbacks{};
    for ( TimestampedCallbacks & item : timesAndCallbacks)
    {
        if (timestampCloseEnough(item.timestamp, timestamp))
        //if (item.timestamp == timestamp) // insert it here
        {
              callbacks.insert(
                    callbacks.end(),
                    item.callbacks.begin(), 
                    item.callbacks.end()
                );
        } 
    }
    return callbacks;
}
 
 

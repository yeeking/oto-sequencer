#include <functional>
#include <vector>

#pragma once

typedef std::vector<std::function<void(std::vector<double>)>> CallbackVector;

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
        void addEvent(long timestamp, std::function<void(std::vector<double>)> callback);
        /** trigger events at the sent timestamp */
        void triggerEventsAtTimestamp(long timestamp);
        /** trigger events now - it will internally work out what the current time is */
        void triggerEventsAtNow();
        /** returns the vector of callbacks attached to the sent timestamp */
        CallbackVector getEventsAtTimestamp(long timestamp);        
    private:
        std::vector<TimestampedCallbacks> timesAndCallbacks;
};

EventQueue::EventQueue()
{

}

void EventQueue::addEvent(long timestamp, std::function<void(std::vector<double>)> callback)
{

}
/** trigger events at the sent timestamp */
void EventQueue::triggerEventsAtTimestamp(long timestamp)
{

}
/** trigger events now - it will internally work out what the current time is */
void EventQueue::triggerEventsAtNow()
{

}
/** returns the vector of callbacks attached to the sent timestamp */
CallbackVector EventQueue::getEventsAtTimestamp(long timestamp)
{
    return CallbackVector{};
}
 
 

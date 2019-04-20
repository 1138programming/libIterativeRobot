#include "EventListener.h"
#include "EventScheduler.h"

using namespace libIterativeRobot;

EventListener::EventListener() {
    // Adds the EventListener instance to the event scheduler
    EventScheduler::getInstance()->addEventListener(this);
}

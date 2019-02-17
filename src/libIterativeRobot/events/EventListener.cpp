#include "libIterativeRobot/events/EventListener.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

EventListener::EventListener() {
    // Adds the EventListener instance to the event scheduler
    EventScheduler::getInstance()->addEventListener(this);
}
#include "libIterativeRobot/subsystems/Subsystem.h"
#include "libIterativeRobot/events/EventScheduler.h"
#include "libIterativeRobot/commands/Command.h"

using namespace libIterativeRobot;

size_t Subsystem::instances = 0;

Subsystem::Subsystem() {
  EventScheduler::getInstance()->trackSubsystem(this);
  instances++;
}

void Subsystem::setDefaultCommand(Command *aCommand) {
  aCommand->priority = Command::DefaultCommandPriority; // Give the default command the lowest possible priority
  this->defaultCommand = aCommand;
  EventScheduler::getInstance()->addCommand(aCommand);
}

void Subsystem::initDefaultCommand() {
  // Setup up a default command here
}

Command* Subsystem::getDefaultCommand() {
  return this->defaultCommand;
}

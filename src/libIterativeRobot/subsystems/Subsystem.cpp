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
  aCommand->requires(this);
  this->defaultCommand = aCommand;
  aCommand->run();
}

Command* Subsystem::getDefaultCommand() {
  return this->defaultCommand;
}

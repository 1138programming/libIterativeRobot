#include "./Command.h"
#include "../subsystems/Subsystem.h"
#include "../events/EventScheduler.h"

using namespace libIterativeRobot;

Command::Command() {
}

void Command::requires(Subsystem* aSubsystem) {
  if (std::find(subsystemRequirements.begin(), subsystemRequirements.end(), aSubsystem) == subsystemRequirements.end()) {
    subsystemRequirements.push_back(aSubsystem);
  }
}

std::vector<Subsystem*>& Command::getRequirements() {
  return this->subsystemRequirements;
}
/*

  Currently removed due to incompatibilities with the current EventScheduler
  May be Re-Added later on once bugs are ironed out

bool Command::canBeInterruptedBy(Command* aCommand) {
  return aCommand->priority > this->priority;
}
*/

void Command::run() {
  this->status = Status::Idle;
  EventScheduler::getInstance()->addCommand(this);
}

void Command::stop() {
  EventScheduler::getInstance()->removeCommand(this);
}

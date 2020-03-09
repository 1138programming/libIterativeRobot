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

void Command::run() {
  this->status = Status::Idle;
  EventScheduler::getInstance()->addCommand(this);
}

void Command::stop() {
  EventScheduler::getInstance()->removeCommand(this);
}

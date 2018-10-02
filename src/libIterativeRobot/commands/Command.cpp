#include "./Command.h"
#include "../subsystems/Subsystem.h"
#include "../events/EventScheduler.h"

using namespace libIterativeRobot;

Command::Command() {
}

void Command::requires(Subsystem* aSubsystem) {
  subsystemRequirements.push_back(aSubsystem);
}

std::vector<Subsystem*>& Command::getRequirements() {
  return this->subsystemRequirements;
}

bool Command::canRun() {
  return true;
}

void Command::initialize() {

}

void Command::execute() {

}

bool Command::isFinished() {
  return true;
}

void Command::end() {

}

void Command::interrupted() {

}

/*

  Currently removed due to incompatibilities with the current EventScheduler
  May be Re-Added later on once bugs are ironed out

bool Command::canBeInterruptedBy(Command* aCommand) {
  return aCommand->priority > this->priority;
}
*/

void Command::run() {
  EventScheduler::getInstance()->addCommand(this);
}

void Command::stop() {
  EventScheduler::getInstance()->removeCommand(this);
}

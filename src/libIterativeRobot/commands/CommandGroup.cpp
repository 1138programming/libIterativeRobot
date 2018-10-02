#include "./CommandGroup.h"
#include "../events/EventScheduler.h"
#include <algorithm>

using namespace libIterativeRobot;

CommandGroup::CommandGroup() {
}

std::vector<Subsystem*>& CommandGroup::getRequirements() {
  return requirements[sequentialIndex];
}

bool CommandGroup::canRun() {
  // Loops through the current sequential step and checks if each command and command group can run
  for (size_t i = 0; i < commands[sequentialIndex].size(); i++) {
    if (!commands[sequentialIndex][i]->canRun())
      return false; // If any cannot run, the command group cannot run
  }
  return true; // If all can run, the command group can run
}

void CommandGroup::initialize() {
  sequentialIndex = 0; // Initializes the sequential index to 0

  // Loops through the added 2d vector and sets each element to 0
  for (size_t i = 0; i < commands.size(); i++) {
    for (size_t j = 0; j < commands.size(); j++) {
      added[i][j] = 0;
    }
  }
}

void CommandGroup::execute() {
  bool sequentialFinished = true; // Boolean to check if the current sequential step is finished
  bool sequentialInterrupted = false; // Boolean to check if the current sequential step has been interrupted
  Command* command; // Pointer to a command or command group

  // Loops through the commands and command groups in the current sequential step
  for (size_t i = 0; i < commands[sequentialIndex].size(); i++) {
    command = commands[sequentialIndex][i]; // Sets command to the command or command group the for loop is accessing

    // If the current command has not been added to the event scheduler, add it
    if (!added[sequentialIndex][i]) {
      command->run(); // Add the current command or command group to the event scheduler
      added[sequentialIndex][i] = 1; // Set the element in the added 2d vector corresponding to the current command or command group to 1
      sequentialFinished = false; // The current sequential step is not finished, so set sequentialFinished to false
    } else { // Otherwise, check the command's status
      // If the command's status is not Finished, then the current sequential step is not finished
      if (command->status != Finished) {
        sequentialFinished = false;
      }

      // If the command's status is interrupted or the command was added but is not running and has not finished (indicating it could not run because of a higher priority command), then set sequentialInterrupted to true
      if (command->status == Interrupted || (command->status != Running && command->status != Finished)) {
        sequentialInterrupted = true;
      }
    }
  }

  //Updates the command group's status based on sequentialInterrupted and sequentialFinished
  if (sequentialInterrupted) status = Interrupted;
  if (sequentialFinished) sequentialIndex++; // If the current sequential step is finished, the command group moves on to the next sequential step
}

bool CommandGroup::isFinished() {
  // Checks if the command group has finished all of its sequential steps
  return !(sequentialIndex < commands.size());
}

void CommandGroup::end() {
  // Resets sequentialIndex to 0
  sequentialIndex = 0;
}

void CommandGroup::interrupted() {
  // Resets the command group's status to idle to let it run again in the future
  status = Idle;

  // Loops through the sequential step and stop any commands and command groups still running
  for (size_t i = 0; i < commands[sequentialIndex].size(); i++) {
    commands[sequentialIndex][i]->stop();
  }

  // Resets sequentialIndex to 0
  sequentialIndex = 0;
}

void CommandGroup::addSequentialCommand(Command* aCommand) {
  std::vector<Command*> commandList;
  std::vector<Subsystem*> requirementList;
  std::vector<int> addedList;

  commandList.push_back(aCommand);
  requirementList.insert(requirementList.end(), aCommand->getRequirements().begin(), aCommand->getRequirements().end());
  addedList.push_back(0);

  this->commands.push_back(commandList);
  this->requirements.push_back(requirementList);
  this->added.push_back(addedList);
}

void CommandGroup::addParallelCommand(Command *aCommand) {
  this->commands.back().push_back(aCommand);
  this->requirements.back().insert(this->requirements.back().end(), aCommand->getRequirements().begin(), aCommand->getRequirements().end());
  this->added.back().push_back(0);
}


void CommandGroup::run() {
  // Adds the command group to the event scheduler
  EventScheduler::getInstance()->addCommandGroup(this);
}

void CommandGroup::stop() {
  // Removes the command group from the event scheduler
  EventScheduler::getInstance()->removeCommandGroup(this);
}

#include "libIterativeRobot/events/EventScheduler.h"
#include <algorithm>

using namespace libIterativeRobot;

using pros::c::delay; // Access to delay();

EventScheduler* EventScheduler::instance = NULL;

EventScheduler::EventScheduler() {
}

void EventScheduler::update() {
  // Calls each event listener's check conditions functoin
  for (EventListener* listener : eventListeners) {
    listener->checkConditions();
  }

  // Initializes each subsystem's default command
  for (Subsystem* subsystem : subsystems) {
    subsystem->initDefaultCommand();
  }
  subsystems.clear(); // Clears subsystems vector so that default commands are initialized only once

  // The following chunk of code from lines 28-92 schedules command groups, running those that can run, finishing those that are finished, and interrupting those that have been interrupted
  std::vector<Subsystem*> usedSubsystems; // Vector keeping track of which subsystems have already been claimed by a command or command group
  CommandGroup* commandGroup; // Pointer to a command group
  bool canRun; // Stores whether each command or command group can run or not

  // If the command group queue is not empty, loop through it and schedule command groups
  if (commandGroupQueue.size() != 0) {
    // Loops backwards through the command group queue. As a result, more recently added command groups are prioritized
    for (int i = commandGroupQueue.size() - 1; i >= 0; i--) {
      commandGroup = commandGroupQueue[i]; // Sets commandGroup to the command group currently being checked for run-ability

      // If the command group's status is interrupted, the command group's interrupted function is called and it is removed from the command group queue
      if (commandGroup->status == Interrupted) {
        commandGroup->interrupted();
        commandGroupQueue.erase(commandGroupQueue.begin() + i);
        continue; // Skips over the rest of the logic for the current command group
      }

      canRun = commandGroup->canRun(); // Sets canRun to the result of the command group's canRun() function
      std::vector<Subsystem*>& commandGroupRequirements = commandGroup->getRequirements(); // Vector storing the command group's requirements

      // Checks whether the command group can run based off of the its requirements and priority
      if ((usedSubsystems.size() == numSubsystems && commandGroupRequirements.size() != 0) || !canRun) {
        canRun = false;
      } else {
        // Loops through the command group's requirements
        for (Subsystem* aSubsystem : commandGroupRequirements) {
          // If the any requirement from the current command group is already in use by a higher priority command group, the command group cannot run
          if (std::find(usedSubsystems.begin(), usedSubsystems.end(), aSubsystem) != usedSubsystems.end()) {
            canRun = false;
            break; // There is no need to check the rest of the requirements
          }
        }
      }

      // Calls the command group's appropriate functions based off of whether it can run
      if (canRun) {
        // Adds the command group's requirements to the list of requirements that are in use
        usedSubsystems.insert(usedSubsystems.end(), commandGroupRequirements.begin(), commandGroupRequirements.end());

        // If the command group is not running, initialize it first
        if (commandGroup->status != Running) {
          commandGroup->initialize();
          commandGroup->status = Running;
        }

        commandGroup->execute(); // Call the command group's execute function

        // If the command group is finished, call its end() function and remove it from the command group queue
        if (commandGroup->isFinished()) {
          commandGroup->end();
          commandGroup->status = Finished;
          commandGroupQueue.erase(commandGroupQueue.begin() + i);
        }
      } else {
        // If the command group is running, call its interrupted() function
        if (commandGroup->status == Running) {
          commandGroup->interrupted();
          commandGroup->status = Interrupted;
        }

        // Remove the command group from the queue
        commandGroupQueue.erase(commandGroupQueue.begin() + i);
      }
    }
  }

  // The following chunk of code from lines 93-155 schedules commands, running those that can run, finishing those that are finished, and interrupting those that have been interrupted
  usedSubsystems.clear(); // Clear the used subsystems vector to prepare it for use for scheduling commands
  Command* command; // Pointer to a command

  // If the command queue size is not empty, loop through it and schedule commands
  if (commandQueue.size() != 0) {
    // Loops backwards through the command queue. The queue is ordered from lowest priority to highes priority
    for (int i = commandQueue.size() - 1; i >= 0; i--) {
      command = commandQueue[i]; // Sets command to the command currently being checked for run-ability
      canRun = command->canRun(); // Sets canRun to the result of the command's canRun() function
      std::vector<Subsystem*>& commandRequirements = command->getRequirements(); // Vector storing the command group's requirements

      // Checks whether the command can run based off of the its requirements and priority
      if ((usedSubsystems.size() == numSubsystems && commandRequirements.size() != 0) || !canRun) {
        // Shortcut to not iterate through the usedSubsystems vector if all subsystems are being used and the command requires one or more subsystem, or the command cannot run
        canRun = false;
      } else {
        // Loops through the command's requirements
        for (Subsystem* aSubsystem : commandRequirements) {
          // If the any requirement from the command is already in use by a higher priority command, the command cannot run
          if (std::find(usedSubsystems.begin(), usedSubsystems.end(), aSubsystem) != usedSubsystems.end()) {
            canRun = false;
            break;
          }
        }
      }

      // Calls the command's appropriate functions based off of whether it can run
      if (canRun) {
        // Adds the command's requirements to the list of requirements that are in use
        usedSubsystems.insert(usedSubsystems.end(), commandRequirements.begin(), commandRequirements.end());

        // If the command group is not running, initialize it first
        if (command->status != Running) {
          command->initialize();
          command->status = Running;
        }

        command->execute(); // Call the command's execute function

        // If the command is finished, call its end() function and remove it from the command queue if it is not a default command
        if (command->isFinished()) {
          command->end();
          command->status = Finished;
          if (command->priority > 0) {
            commandQueue.erase(commandQueue.begin() + i);
          }
        }
      } else {
        // If the command group is running, call its interrupted() function
        if (command->status == Running) {
          command->interrupted();
          command->status = Interrupted;
        }

        // Remove the command group from the queue if it is not a default command
        if (command->priority > 0) {
          commandQueue.erase(commandQueue.begin() + i);
        }
      }
    }
  }
  delay(5);
}

void EventScheduler::addCommand(Command* command) {
  // Makes sure the command is not in the queue yet
  if (!commandInQueue(command)) {
    // The command is added into the queue in order of priority. More recent commands take priority if the priorities are the same
    for (size_t i = 0; i < commandQueue.size(); i++) {
      if (command->priority < commandQueue[i]->priority) {
        commandQueue.insert(commandQueue.begin() + i, command);
        return;
      }
    }
    commandQueue.push_back(command);
  }
}

void EventScheduler::addCommandGroup(CommandGroup* commandGroupToRun) {
  // If the command group is not already in the queue, the command group is added to the end of the queue
  if (!commandGroupInQueue(commandGroupToRun)) {
    commandGroupQueue.push_back(commandGroupToRun);
  }
}

void EventScheduler::removeCommand(Command* commandToRemove) {
  // Interrupts the command being removed
  commandToRemove->interrupted();

  // Removes the command
  size_t index = std::find(commandQueue.begin(), commandQueue.end(), commandToRemove) - commandQueue.begin();
  if (index >= commandQueue.size())
    return;
  commandQueue.erase(commandQueue.begin() + index);
}

void EventScheduler::removeCommandGroup(CommandGroup* commandGroupToRemove) {
  // Interrupts the command group being removed
  commandGroupToRemove->interrupted();

  // Removes the command group
  size_t index = std::find(commandGroupQueue.begin(), commandGroupQueue.end(), commandGroupToRemove) - commandGroupQueue.begin();
  if (index >= commandGroupQueue.size())
    return;
  commandGroupQueue.erase(commandGroupQueue.begin() + index);
}

void EventScheduler::clearCommandQueue() {
  // Removes every command from the command queue using removeCommand()
  for (size_t i = 0; i < commandQueue.size(); i++) {
    removeCommand(commandQueue[i]);
  }
}

void EventScheduler::clearCommandGroupQueue() {
  // Removes every command group from the command group queue using removeCommandGroup()
  for (size_t i = 0; i < commandGroupQueue.size(); i++) {
    removeCommandGroup(commandGroupQueue[i]);
  }
}

void EventScheduler::addEventListener(EventListener* eventListener) {
  this->eventListeners.push_back(eventListener);
}

void EventScheduler::trackSubsystem(Subsystem *aSubsystem) {
  this->subsystems.push_back(aSubsystem);
  numSubsystems++; // Keeps track of the number of subsystems
}

bool EventScheduler::commandInQueue(Command* aCommand) {
  return std::find(commandQueue.begin(), commandQueue.end(), aCommand) != commandQueue.end();
}

bool EventScheduler::commandGroupInQueue(CommandGroup* aCommandGroup) {
  return std::find(commandGroupQueue.begin(), commandGroupQueue.end(), aCommandGroup) != commandGroupQueue.end();
}

EventScheduler* EventScheduler::getInstance() {
    if (instance == NULL) {
        instance = new EventScheduler();
    }
    return instance;
}

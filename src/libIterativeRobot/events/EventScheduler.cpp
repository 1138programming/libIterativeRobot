#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

using pros::c::delay; // Access to delay();

EventScheduler* EventScheduler::instance = NULL;

EventScheduler::EventScheduler() {
}

void EventScheduler::checkEventListeners() {
  // Calls each event listener's check conditions function
  for (EventListener* listener : eventListeners) {
    listener->checkConditions();
  }
}

void EventScheduler::addDefaultCommands() {
  // Initializes each subsystem's default command
  if (!defaultAdded) {
    for (Subsystem* subsystem : subsystems) {
      subsystem->initDefaultCommand();
    }
    defaultAdded = true;
  }
}

void EventScheduler::scheduleCommandGroups(std::vector<CommandGroup*>* commandGroups) {
  if (commandGroups->size() != 0) {
    CommandGroup* commandGroup;
    for (int i = commandGroups->size() - 1; i >= 0; i--) {
      commandGroup = (*commandGroups)[i]; // Sets commandGroup to the command group currently being checked

      // If the command group's status is interrupted, the command group's interrupted function is called and it is removed from the command group queue
      if (commandGroup->status == Status::Interrupted) {
        commandGroup->interrupted();
        commandGroups->erase(commandGroups->begin() + i);
        continue; // Skips over the rest of the logic for the current command group
      }

      // If the command group is not running, initialize it first
      if (commandGroup->status != Status::Running) {
        commandGroup->initialize();
      }

      commandGroup->execute(); // Call the command group's execute function

      // If the command group is finished, call its end() function and remove it from the command group queue
      if (commandGroup->isFinished()) {
        commandGroup->end();
        commandGroups->erase(commandGroups->begin() + i);
        //printf("Command group erased, new size is %d, queue size is %d\n", commandGroups->size(), commandGroupQueue.size());
      }
    }
  }
}

void EventScheduler::update() {
  //printf("EventScheduler update\n");
  checkEventListeners();
  addDefaultCommands();

  // Schedules all command groups
  queueCommandGroups(); // Dumps the contents of the commandGroupBuffer into the commandGroupQueue

  scheduleCommandGroups(&commandGroupQueue); // Schedule the commands in the commandGroupQueue
  //printf("commandGroupBuffer: %d, intermediateGroupBuffer: %d, commandGroupQueue: %d\n", commandGroupBuffer.size(), intermediateGroupBuffer.size(), commandGroupQueue.size());
  while (commandGroupBuffer.size() != 0) { // Schedule any CommandGroups added to the commandGroupBuffer
    toIntermediateBuffer(); // Dump contents of the commandGroupBuffer into the intermediateGroupBuffer
    scheduleCommandGroups(&intermediateGroupBuffer); // Schedule the commands in the intermediateGroupBuffer
    toGroupQueue(); // Dump the contents of the intermediateGroupBuffer into the commandGroupQueue
  }

  //Schedule all commands, running those that can run, finishing those that are finished, and interrupting those that have been interrupted
  std::vector<Subsystem*> usedSubsystems; // Vector keeping track of which subsystems have already been claimed by a command or command group
  bool canRun; // Stores whether each command or command group can run or not
  toExecute.clear();
  indexes.clear();
  Command* command;

  // Dumps the contents of the commandBuffer into the commandQueue
  queueCommands();

  // If the command queue size is not empty, loop through it and schedule commands
  if (commandQueue.size() != 0) {
    // Loops backwards through the command queue. The queue is ordered from lowest priority to highest priority, and commands with the same priority are ordered from most recent to oldest
    for (int i = commandQueue.size() - 1; i >= 0; i--) {
      command = commandQueue[i];
      canRun = command->canRun();
      std::vector<Subsystem*>& commandRequirements = command->getRequirements();

      // Checks whether the command can run based off of its requirements and priority
      if ((usedSubsystems.size() == numSubsystems && commandRequirements.size() != 0) || !canRun) {
        // Shortcut to not iterate through the usedSubsystems vector if all subsystems are being used and the command requires one or more subsystem, or the command cannot run
        canRun = false;
      } else {
        // Loops through the command's requirements
        for (Subsystem* aSubsystem : commandRequirements) {
          // If any requirement from the command is already in use by a higher priority command, the command cannot run
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

        // Stores the command in another vector to by executed later. It is not executed here because all interrupted methods need to run before any initialize or execute methods can run
        toExecute.push_back(command);
        indexes.push_back(i);
      } else {
        // If the command group is running, call its interrupted() function
        if (command->status == Status::Running) {
          command->status = Status::Interrupted;
          command->interrupted();
        }

        // Set the command to be removed from the queue if it is not a default command
        if (command->priority > 0) {
          commandQueue[i] = NULL;
        }
      }
    }

    // Loop through the toExecute vector and initialize, execute, or end the commands as necessary
    unsigned int i = 0;
    for (Command* command : toExecute) {
      // If the command group is not running, initialize it first
      if (command->status != Status::Running) {
        command->initialize();
        command->status = Status::Running;
      }

      command->execute();

      // If the command is finished, call its end() function and remove it from the command queue if it is not a default command
      if (command->isFinished()) {
        command->end();
        command->status = Status::Finished;
        if (command->priority > 0) {
          commandQueue[indexes[i]] = NULL;
        }
      }
      i++;
    }

    // Remove NULL values from the commandQueue
    for (int i = commandQueue.size() - 1; i >= 0; i--) {
      if (commandQueue[i] == NULL) {
        commandQueue.erase(commandQueue.begin() + i);
      }
    }
  }

  delay(5);
}

void EventScheduler::addCommand(Command* command) {
  // Makes sure the command is not in the scheduler yet and then adds it to the buffer
  if (!commandInScheduler(command)) {
    commandBuffer.push_back(command);
  } else {
      //say("Command found in scheduler, it was not added\n");
  }
}

void EventScheduler::addCommandGroup(CommandGroup* commandGroup) {
  // If the command group is not already in the scheduler, the command group is added to the end of the buffer
  if (!commandGroupInScheduler(commandGroup)) {
    commandGroupBuffer.push_back(commandGroup);
  }
}

void EventScheduler::queueCommands() {
  // Adds the commands in the command buffer into the command queue in order of priority
  //say("CommandBuffer size is %d\n", commandBuffer.size());
  for (Command* command : commandBuffer) {
    for (size_t i = 0; i < commandQueue.size(); i++) {
      if (command->priority < commandQueue[i]->priority) {
        commandQueue.insert(commandQueue.begin() + i, command);
        break;
      }
    }
    //say("Command added to command queue\n");
    commandQueue.push_back(command);
  }

  // Clears the command buffer
  commandBuffer.clear();
}

void EventScheduler::toIntermediateBuffer() {
  // Adds all command groups in the command group buffer into the intermediate group buffer
  for (CommandGroup* commandGroup : commandGroupBuffer)
    intermediateGroupBuffer.push_back(commandGroup);

  // Clears the command group buffer
  commandGroupBuffer.clear();
}

void EventScheduler::toGroupQueue() {
  // Adds all command groups in the intermediate group buffer into the command group queue
  for (CommandGroup* commandGroup : intermediateGroupBuffer)
    commandGroupQueue.push_back(commandGroup);

  // Clears the intermediate group buffer
  intermediateGroupBuffer.clear();
}

void EventScheduler::queueCommandGroups() {
  // Adds all command groups in the command group buffer into the command group queue
  for (CommandGroup* commandGroup : commandGroupBuffer)
    commandGroupQueue.push_back(commandGroup);

  // Clears the command group buffer
  commandGroupBuffer.clear();
}

void EventScheduler::removeCommand(Command* command) {
  // Interrupts the command being removed
  command->interrupted();
  command->status = Status::Interrupted;

  // Removes the command
  size_t index = std::find(commandBuffer.begin(), commandBuffer.end(), command) - commandBuffer.begin(); // Get the index of the command in the commandBuffer vector
  if (index >= commandBuffer.size()) { // If the command is not in the commandBuffer vector, check in the commandQueue vector
    index = std::find(commandQueue.begin(), commandQueue.end(), command) - commandQueue.begin(); // Get the index of the command in the commandQueue vector
    if (index >= commandQueue.size()) // If the command is not in the commandQueue vector, return
      return;
    commandQueue.erase(commandQueue.begin() + index); // Remove command from commandQueue
  } else {
    commandBuffer.erase(commandBuffer.begin() + index); // Remove command from commandBuffer
  }
}

void EventScheduler::removeCommandGroup(CommandGroup* commandGroup) {
  // Interrupts the command group being removed
  commandGroup->interrupted();

  // Removes the command group
  size_t index = std::find(commandGroupBuffer.begin(), commandGroupBuffer.end(), commandGroup) - commandGroupBuffer.begin();  // Get the index of the command group in the commandGroupBuffer vector
  if (index >= commandGroupBuffer.size()) { // If the command group is not in the commandGroupBuffer vector, check in the commandGroupQueue vector
    index = std::find(commandGroupQueue.begin(), commandGroupQueue.end(), commandGroup) - commandGroupQueue.begin(); // Get the index of the command group in the commandGroupsQueue vector
    if (index >= commandGroupQueue.size()) // If the command group is not in the commandGroupQueue vector, return
      return;
    commandGroupQueue.erase(commandGroupQueue.begin() + index); // Remove command group from commandGroupQueue
  } else {
    commandGroupBuffer.erase(commandGroupBuffer.begin() + index); // Remove command group from commandGroupBuffer
  }
}

void EventScheduler::clearScheduler() {
  for (Command* command : commandBuffer) {
    command->interrupted();
  }

  for (Command* command : commandQueue) {
    command->interrupted();
  }

  commandBuffer.clear();
  commandQueue.clear();
  commandGroupBuffer.clear();
  commandGroupQueue.clear();
}

void EventScheduler::addEventListener(EventListener* eventListener) {
  this->eventListeners.push_back(eventListener);
}

void EventScheduler::trackSubsystem(Subsystem *aSubsystem) {
  this->subsystems.push_back(aSubsystem);
  numSubsystems++; // Keeps track of the number of subsystems
}

bool EventScheduler::commandInScheduler(Command* aCommand) {
  bool inCommandsToBeAdded = std::find(commandBuffer.begin(), commandBuffer.end(), aCommand) != commandBuffer.end();
  bool inCommandQueue = std::find(commandQueue.begin(), commandQueue.end(), aCommand) != commandQueue.end();
  return inCommandsToBeAdded || inCommandQueue;
}

bool EventScheduler::commandGroupInScheduler(CommandGroup* aCommandGroup) {
  bool inCommandGroupsToBeAdded = std::find(commandGroupBuffer.begin(), commandGroupBuffer.end(), aCommandGroup) != commandGroupBuffer.end();
  bool inCommandGroupQueue = std::find(commandGroupQueue.begin(), commandGroupQueue.end(), aCommandGroup) != commandGroupQueue.end();
  return inCommandGroupsToBeAdded || inCommandGroupQueue;
}

void EventScheduler::initialize(bool noDefaultCommands) {
  clearScheduler();
  defaultAdded = noDefaultCommands;
}

EventScheduler* EventScheduler::getInstance() {
    if (instance == NULL) {
        instance = new EventScheduler();
    }
    return instance;
}

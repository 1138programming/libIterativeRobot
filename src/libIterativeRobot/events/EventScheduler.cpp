#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

using pros::c::delay; // Access to delay();

EventScheduler* EventScheduler::instance = NULL;

EventScheduler::EventScheduler() {
}

void EventScheduler::update() {
  //say("Starting event scheduler, checking EventListeners\n");
  //wait(1000);

  // Calls each event listener's check conditions function
  for (EventListener* listener : eventListeners) {
    listener->checkConditions();
  }

  //say("Initializing default commands\n");
  //wait(1000);

  // Initializes each subsystem's default command
  if (!defaultAdded) {
    for (Subsystem* subsystem : subsystems) {
      subsystem->initDefaultCommand();
    }
    defaultAdded = true;
  }
  queueCommandGroups();

  //say("Starting on scheduling CommandGroups\n");
  //wait(1000);

  //Schedule command groups, running those that can run, finishing those that are finished, and interrupting those that have been interrupted
  std::vector<Subsystem*> usedSubsystems; // Vector keeping track of which subsystems have already been claimed by a command or command group
  CommandGroup* commandGroup; // Pointer to a command group
  bool canRun; // Stores whether each command or command group can run or not

  // If the command group queue is not empty, loop through it and schedule command groups
  if (commandGroupQueue.size() != 0) {
    // Loops backwards through the command group queue. As a result, more recently added command groups are prioritized
    for (int i = commandGroupQueue.size() - 1; i >= 0; i--) {
      commandGroup = commandGroupQueue[i]; // Sets commandGroup to the command group currently being checked

      // If the command group's status is interrupted, the command group's interrupted function is called and it is removed from the command group queue
      if (commandGroup->status == Status::Interrupted) {
        commandGroup->interrupted();
        commandGroupQueue.erase(commandGroupQueue.begin() + i);
        continue; // Skips over the rest of the logic for the current command group
      }

      canRun = commandGroup->canRun(); // Sets canRun to the result of the command group's canRun() function

      std::vector<Subsystem*>& commandGroupRequirements = commandGroup->getRequirements(); // Vector storing the command group's requirements

      // Checks whether the command group can run based off of its requirements
      if ((usedSubsystems.size() == numSubsystems && commandGroupRequirements.size() != 0) || !canRun) {
        canRun = false;
      } else {
        // Loops through the command group's requirements
        for (Subsystem* aSubsystem : commandGroupRequirements) {
          // If any requirement from the current command group is already in use by a more recently added command group, the command group cannot run
          if (std::find(usedSubsystems.begin(), usedSubsystems.end(), aSubsystem) != usedSubsystems.end()) {
            canRun = false;
            break; // There is no need to check the rest of the requirements
          }
        }
      }

      // Calls the command group's appropriate functions based off of whether it can run and its status
      if (canRun) {
        // Adds the command group's requirements to the list of requirements that are in use
        usedSubsystems.insert(usedSubsystems.end(), commandGroupRequirements.begin(), commandGroupRequirements.end());

        // If the command group is not running, initialize it first
        if (commandGroup->status != Status::Running) {
          commandGroup->initialize();
        }

        commandGroup->execute(); // Call the command group's execute function

        // If the command group is finished, call its end() function and remove it from the command group queue
        if (commandGroup->isFinished()) {
          commandGroup->end();
          commandGroupQueue.erase(commandGroupQueue.begin() + i);
        }
      } else {
        // If the command group is running, call its interrupted() function
        if (commandGroup->status == Status::Running) {
          commandGroup->interrupted();
        }

        // Remove the command group from the queue
        commandGroupQueue.erase(commandGroupQueue.begin() + i);
      }
    }
  }

  //say("Starting on scheduling Commands\n");
  //wait(1000);

  // Adds the Commands in the commandBuffer into the commandQueue
  queueCommands();

  //say("Initializing vectors\n");
  //wait(1000);

  //Schedule commands, running those that can run, finishing those that are finished, and interrupting those that have been interrupted
  usedSubsystems.clear();
  toExecute.clear();
  indexes.clear();
  Command* command;

  // If the command queue size is not empty, loop through it and schedule commands
  if (commandQueue.size() != 0) {
    //say("Looping through command queue, size is %d\n", commandQueue.size());
    //wait(1000);
    // Loops backwards through the command queue. The queue is ordered from lowest priority to highest priority, and commands with the same priority are ordered from most recent to oldest
    for (int i = commandQueue.size() - 1; i >= 0; i--) {
      //say("0, i is %d\n", i);
      //wait(1000);
      command = commandQueue[i];
      //say("Command address is %p\n", command);
      //wait(1000);
      //say("1\n");
      //wait(1000);
      canRun = command->canRun();
      //say("2\n");
      //wait(1000);
      std::vector<Subsystem*>& commandRequirements = command->getRequirements();

      // Checks whether the command can run based off of its requirements and priority
      //say("3\n");
      //wait(1000);
      if ((usedSubsystems.size() == numSubsystems && commandRequirements.size() != 0) || !canRun) {
        //say("4\n");
        //wait(1000);
        // Shortcut to not iterate through the usedSubsystems vector if all subsystems are being used and the command requires one or more subsystem, or the command cannot run
        canRun = false;
      } else {
        //say("5\n");
        //wait(1000);
        // Loops through the command's requirements
        for (Subsystem* aSubsystem : commandRequirements) {
          //say("6\n");
          //wait(1000);
          // If any requirement from the command is already in use by a higher priority command, the command cannot run
          if (std::find(usedSubsystems.begin(), usedSubsystems.end(), aSubsystem) != usedSubsystems.end()) {
            //say("7\n");
            //wait(1000);
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

    //say("Looping through and executing commands as necessary\n");
    //wait(1000);
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

void EventScheduler::addCommandGroup(CommandGroup* commandGroupToRun) {
  // If the command group is not already in the scheduler, the command group is added to the end of the buffer
  if (!commandGroupInScheduler(commandGroupToRun)) {
    commandGroupBuffer.push_back(commandGroupToRun);
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

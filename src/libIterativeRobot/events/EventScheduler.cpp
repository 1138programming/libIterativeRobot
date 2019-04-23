#include "libIterativeRobot/events/EventScheduler.h"
#include <algorithm>

using namespace libIterativeRobot;

using pros::c::delay; // Access to delay();

EventScheduler* EventScheduler::instance = NULL;

EventScheduler::EventScheduler() {
}

void EventScheduler::update() {
  toInitialize.clear();
  toExecute.clear();
  toInterrupt.clear();
  toEnd.clear();

  // Calls each event listener's check conditions function
  for (EventListener* listener : eventListeners) {
    listener->checkConditions();
  }

  // Initializes each subsystem's default command
  if (!defaultAdded) {
    for (Subsystem* subsystem : subsystems) {
        subsystem->initDefaultCommand();
      }
      defaultAdded = true;
  }

  //comment("Adding command groups to the queue, command group buffer size is %d\n", commandGroupBuffer.size());
  //pros::wait(1000);
  queueCommandGroups();

  // The following chunk of code from lines 28-92 schedules command groups, running those that can run, finishing those that are finished, and interrupting those that have been interrupted
  std::vector<Subsystem*> usedSubsystems; // Vector keeping track of which subsystems have already been claimed by a command or command group
  CommandGroup* commandGroup; // Pointer to a command group
  bool canRun; // Stores whether each command or command group can run or not

  //comment("Scheduling command groups\n");
  //pros::wait(1000);

  // If the command group queue is not empty, loop through it and schedule command groups
  if (commandGroupQueue.size() != 0) {
    // Loops backwards through the command group queue. As a result, more recently added command groups are prioritized
    //comment("Command group queue size is %d\n", commandGroupQueue.size());
    //pros::wait(1000);
    for (int i = commandGroupQueue.size() - 1; i >= 0; i--) {
      //comment("Looping through command group queue, i is %d\n", i);
      //pros::wait(1000);

      commandGroup = commandGroupQueue[i]; // Sets commandGroup to the command group currently being checked for run-ability

      // If the command group's status is interrupted, the command group's interrupted function is called and it is removed from the command group queue
      if (commandGroup->status == Interrupted) {
        //comment("Command group interrupted\n");
        //pros::wait(1000);
        commandGroup->interrupted();
        commandGroupQueue.erase(commandGroupQueue.begin() + i);
        continue; // Skips over the rest of the logic for the current command group
      }

      //comment("Checking command group's can run function, command group address is %x\n", commandGroup);
      //pros::wait(1000);
      canRun = commandGroup->canRun(); // Sets canRun to the result of the command group's canRun() function
      //canRun = true;
      //comment("Done checking command group's can run function, value is %d\n", canRun);
      //pros::wait(1000);
      std::vector<Subsystem*>& commandGroupRequirements = commandGroup->getRequirements(); // Vector storing the command group's requirements

      // Checks whether the command group can run based off of the its requirements and priority
      if ((usedSubsystems.size() == numSubsystems && commandGroupRequirements.size() != 0) || !canRun) {
        //comment("Command group cannot run\n");
        //pros::wait(1000);
        canRun = false;
      } else {
        // Loops through the command group's requirements
        //comment("Looping through command group requirements\n");
        //pros::wait(1000);
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
        //comment("Command group can run\n");
        //pros::wait(1000);
        // Adds the command group's requirements to the list of requirements that are in use
        usedSubsystems.insert(usedSubsystems.end(), commandGroupRequirements.begin(), commandGroupRequirements.end());

        // If the command group is not running, initialize it first
        if (commandGroup->status != Running) {
          commandGroup->initialize();
          //comment("Initialized command group\n");
          //pros::wait(1000);
        }

        commandGroup->execute(); // Call the command group's execute function
        //comment("Command group status is %d\n", commandGroup->status);
        //pros::wait(1000);

        // If the command group is finished, call its end() function and remove it from the command group queue
        if (commandGroup->isFinished()) {
          //comment("Command group is finished\n");
          //pros::wait(1000);
          commandGroup->end();
          commandGroupQueue.erase(commandGroupQueue.begin() + i);
        } else {
          //comment("Command group is not finished\n");
          //pros::wait(1000);
        }
      } else {
        //comment("Command group cannot run\n");
        //pros::wait(1000);
        // If the command group is running, call its interrupted() function
        if (commandGroup->status == Running) {
          //comment("Command group was interrupted\n");
          //pros::wait(1000);
          commandGroup->interrupted();
        }

        // Remove the command group from the queue
        commandGroupQueue.erase(commandGroupQueue.begin() + i);
      }
    }
  }

  //comment("Done scheduling command groups, starting on commands\n");
  //pros::wait(1000);
  //comment("Adding commands to the queue, command buffer size is %d\n", commandBuffer.size());
  //pros::wait(1000);
  queueCommands();

  // The following chunk of code from lines 93-155 schedules commands, running those that can run, finishing those that are finished, and interrupting those that have been interrupted
  usedSubsystems.clear(); // Clear the used subsystems vector to prepare it for use for scheduling commands
  Command* command; // Pointer to a command

  // If the command queue size is not empty, loop through it and schedule commands
  if (commandQueue.size() != 0) {
    //pros::wait(1000);
    //comment("Command queue size is %d, looping through\n", commandQueue.size());
    // Loops backwards through the command queue. The queue is ordered from lowest priority to highes priority
    for (int i = commandQueue.size() - 1; i >= 0; i--) {
      //comment("i is %d\n", i);
      //pros::wait(1000);
      command = commandQueue[i]; // Sets command to the command currently being checked for run-ability
      //comment("Checking if command can run\n");
      //pros::wait(1000);
      canRun = command->canRun(); // Sets canRun to the result of the command's canRun() function
      std::vector<Subsystem*>& commandRequirements = command->getRequirements(); // Vector storing the command group's requirements

      // Checks whether the command can run based off of the its requirements and priority
      if ((usedSubsystems.size() == numSubsystems && commandRequirements.size() != 0) || !canRun) {
        //comment("Command cannot run\n");
        //pros::wait(1000);
        // Shortcut to not iterate through the usedSubsystems vector if all subsystems are being used and the command requires one or more subsystem, or the command cannot run
        canRun = false;
      } else {
        //comment("Looping through command requirements queue\n");
        //pros::wait(1000);
        // Loops through the command's requirements
        for (Subsystem* aSubsystem : commandRequirements) {
          // If the any requirement from the command is already in use by a higher priority command, the command cannot run
          if (std::find(usedSubsystems.begin(), usedSubsystems.end(), aSubsystem) != usedSubsystems.end()) {
            //comment("Requirements conflict, command cannot run\n");
            //pros::wait(1000);
            canRun = false;
            break;
          }
        }
      }

      // Calls the command's appropriate functions based off of whether it can run
      if (canRun) {
        //comment("Command can run\n");
        //pros::wait(1000);

        // Adds the command's requirements to the list of requirements that are in use
        usedSubsystems.insert(usedSubsystems.end(), commandRequirements.begin(), commandRequirements.end());

        // If the command group is not running, initialize it first
        if (command->status != Running) {
          //comment("Initializing command\n");
          //pros::wait(1000);
          //command->initialize();
          // toInitialize.push_back(command);
          command->initialize();
          command->status = Running;
        }

        //command->execute(); // Call the command's execute function
        // toExecute.push_back(command);
        command->execute();

        // If the command is finished, call its end() function and remove it from the command queue if it is not a default command
        if (command->isFinished()) {
          //comment("Command is finished\n");
          //pros::wait(1000);
          //command->end();
          // toEnd.push_back(command);
          command->end();
          command->status = Finished;
          if (command->priority > 0) {
            //comment("Removing command, queue size is %d\n", commandQueue.size());
            //pros::wait(1000);

            commandQueue.erase(commandQueue.begin() + i);

            //comment("Queue size is %d\n", commandQueue.size());
            //pros::wait(1000);

            if (commandQueue.size() > 1) {
              //comment("Address of command in queue is 0x%x", commandQueue[1]);
              //pros::wait(1000);
            }
          }
        }
      } else {
        //comment("Command cannot run\n");
        //pros::wait(1000);
        // If the command group is running, call its interrupted() function
        if (command->status == Running) {
          //command->interrupted();
          toInterrupt.push_back(command);
          command->status = Interrupted;
        }

        // Remove the command group from the queue if it is not a default command
        if (command->priority > 0) {
          commandQueue.erase(commandQueue.begin() + i);
        }
      }
    }
  }
  // else {
  //   comment("No commands in the queue\n");
  //   pros::wait(1000);
  // }

  // for (Command* command : toInterrupt)
  //   command->interrupted();
  // for (Command* command : toInitialize)
  //   command->initialize();
  // for (Command* command : toExecute)
  //   command->execute();
  // for (Command* command : toEnd)
  //   command->end();

  delay(5);
}

void EventScheduler::addCommand(Command* command) {
  // Makes sure the command is not in the scheduler yet and then adds it to the buffer
  if (!commandInScheduler(command)) {
    commandBuffer.push_back(command);
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
  for (Command* command : commandBuffer) {
    for (size_t i = 0; i < commandQueue.size(); i++) {
      if (command->priority < commandQueue[i]->priority) {
        commandQueue.insert(commandQueue.begin() + i, command);
        break;
      }
    }
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
  command->status = Interrupted;

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

void EventScheduler::initialize(bool addDefaultCommands) {
  clearScheduler();
  if (addDefaultCommands) {
    defaultAdded = false;
  }
}

EventScheduler* EventScheduler::getInstance() {
    if (instance == NULL) {
        instance = new EventScheduler();
    }
    return instance;
}

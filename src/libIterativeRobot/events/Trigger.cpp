#include "libIterativeRobot/events/Trigger.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

Trigger::Trigger() {
  // Adds the Trigger instance to the event scheduler
  EventScheduler::getInstance()->addEventListener(this);
}

void Trigger::checkConditions() {
  // Keeps track of the button's current state
  std::int32_t currentButtonState = get();

  // Decides which command or command group to run based on the last state and current state of the button. There are four possiblities
  if (currentButtonState) {
    if (lastState) { // Possibility 1: current state is true and last state is true
      for (Command* command : runWhenActivatedCommands) { // Commands in runHeldCommands are run
        command->run();
      }
      for (Command* command : stopWhenActivatedCommands) { // Commands in stopHeldCommands are stopped
        command->stop();
      }
    } else { // Possibility 2: current state is true and last state is false
      for (Command* command : runWhileInactiveCommands) { // Commands in runPressedCommand are run
        command->run();
      }
      for (Command* command : stopWhileInactiveCommands) { // Commands in stopPressedCommand are stopped
        command->stop();
      }
    }
  } else {
    if (lastState) { // Possibility 3: current state is false and last state is true
      for (Command* whenReleasedCommand : runWhenDeactivatedCommands) { // Commands in runWhenReleasedCommands are run
        whenReleasedCommand->run();
      }
      for (Command* whenReleasedCommand : stopWhenDeactivatedCommands) { // Commands in stopWhenReleasedCommands are stopped
        whenReleasedCommand->stop();
      }
    } else { // Possibility 4: current state is false and last state is false
      for (Command* whileReleasedCommand : runWhileInactiveCommands) { // Commands in runWhileReleasedCommands are run
        whileReleasedCommand->run();
      }
      for (Command* whileReleasedCommand : stopWhileInactiveCommands) { // Commands in stopWhileReleasedCommands are stopped
        whileReleasedCommand->stop();
      }
    }
  }

  // Last state is updated
  lastState = currentButtonState;
}

void Trigger::whenActivated(Command* command, Action action) {
  if (action == RUN) {
    runWhenActivatedCommands.push_back(command);
  } else if (action == STOP) {
    stopWhenActivatedCommands.push_back(command);
  }
}

void Trigger::whileActive(Command* command, Action action) {
  if (action == RUN) {
    runWhileActiveCommands.push_back(command);
  } else if (action == STOP) {
    stopWhileActiveCommands.push_back(command);
  }
}

void Trigger::whenDeactivated(Command* command, Action action) {
  if (action == RUN) {
    runWhenDeactivatedCommands.push_back(command);
  } else if (action == STOP) {
    stopWhenDeactivatedCommands.push_back(command);
  }
}

void Trigger::whileInactive(Command* command, Action action) {
  if (action == RUN) {
    runWhileInactiveCommands.push_back(command);
  } else if (action == STOP) {
    stopWhileInactiveCommands.push_back(command);
  }
}

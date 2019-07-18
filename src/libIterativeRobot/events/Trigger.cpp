#include "libIterativeRobot/events/Trigger.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

Trigger::Trigger() {
}

void Trigger::checkConditions() {
  // Keeps track of the button's current state
  bool currentState = getState();

  // Decides which command or command group to run based on the last state and current state of the button. There are four possiblities
  if (currentState) {
    if (lastState) { // Possibility 1: current state is true and last state is true
      for (Command* command : runWhileActiveCommands) { // Commands in runWhileActiveCommands are run
        command->run();
      }
      for (Command* command : stopWhileActiveCommands) { // Commands in stopWhileActiveCommands are stopped
        command->stop();
      }
    } else { // Possibility 2: current state is true and last state is false
      for (Command* command : runWhenActivatedCommands) { // Commands in runWhenActivatedCommands are run
        command->run();
      }
      for (Command* command : stopWhenActivatedCommands) { // Commands in stopWhenActivatedCommands are stopped
        command->stop();
      }
    }
  } else {
    if (lastState) { // Possibility 3: current state is false and last state is true
      for (Command* command : runWhenDeactivatedCommands) { // Commands in runWhenDeactivatedCommands are run
        command->run();
      }
      for (Command* command : stopWhenDeactivatedCommands) { // Commands in stopWhenDeactivatedCommands are stopped
        command->stop();
      }
    } else { // Possibility 4: current state is false and last state is false
      for (Command* command : runWhileInactiveCommands) { // Commands in runWhileInactiveCommands are run
        command->run();
      }
      for (Command* command : stopWhileInactiveCommands) { // Commands in stopWhileReleasedCommands are stopped
        command->stop();
      }
    }
  }

  // Last state is updated
  lastState = currentState;
}

void Trigger::whenActivated(Command* command, Action action) {
  if (action == Action::RUN) {
    runWhenActivatedCommands.push_back(command);
  } else if (action == Action::STOP) {
    stopWhenActivatedCommands.push_back(command);
  }
}

void Trigger::whileActive(Command* command, Action action) {
  if (action == Action::RUN) {
    runWhileActiveCommands.push_back(command);
  } else if (action == Action::STOP) {
    stopWhileActiveCommands.push_back(command);
  }
}

void Trigger::whenDeactivated(Command* command, Action action) {
  if (action == Action::RUN) {
    runWhenDeactivatedCommands.push_back(command);
  } else if (action == Action::STOP) {
    stopWhenDeactivatedCommands.push_back(command);
  }
}

void Trigger::whileInactive(Command* command, Action action) {
  if (action == Action::RUN) {
    runWhileInactiveCommands.push_back(command);
  } else if (action == Action::STOP) {
    stopWhileInactiveCommands.push_back(command);
  }
}

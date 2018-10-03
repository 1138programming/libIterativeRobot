#include "libIterativeRobot/events/JoystickButton.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

JoystickButton::JoystickButton(pros::Controller* controller, pros::controller_digital_e_t button) {
  this->controller = controller;
  this->button = button;

  // Adds the JoystickButton instance to the event scheduler
  EventScheduler::getInstance()->addEventListener(this);
}

void JoystickButton::checkConditions() {
  // Keeps track of the button's current state
  std::int32_t currentButtonState = controller->get_digital(button);

  // Decides which command or command group to run based on the last state and current state of the button. There are four possiblities
  if (currentButtonState) {
    if (lastState) { // Possibility 1: current state is true and last state is true
      if (heldCommand != NULL) { // heldCommand is run if it is not null
        heldCommand->run();
      }
    } else { // Possibility 2: current state is true and last state is false
      if (pressedCommand != NULL) { // pressedCommand is run if it is not null
        pressedCommand->run();
      }
    }
  } else {
    if (lastState) { // Possibility 3: current state is false and last state is true
      if (depressedCommand != NULL) { // depressedCommand is run if it is not null
        depressedCommand->run();
      }
    } else { // Possibility 4: current state is false and last state is false
      if (releasedCommand != NULL) { // releasedCommand is run if it is not null
        releasedCommand->run();
      }
    }
  }

  // Last state is updated
  lastState = currentButtonState;
}

void JoystickButton::whenPressed(Command* pressedCommand) {
  this->pressedCommand = pressedCommand;
}

void JoystickButton::whileHeld(Command* heldCommand) {
  this->heldCommand = heldCommand;
}

void JoystickButton::whenDepressed(Command* depressedCommand) {
  this->depressedCommand = depressedCommand;
}

void JoystickButton::whileReleased(Command* releasedCommand) {
  this->releasedCommand = releasedCommand;
}

#include "libIterativeRobot/events/JoystickButton.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

JoystickButton::JoystickButton(Joysticks joystick, Buttons button) {
  // Joystick to check can either be the main controller or partner controller
  this->joystickToCheck = (int)joystick;

  // Sets the buttonGroup and button based on the enum value passed to the JoystickButton instance
  switch (button) {
    case Btn5U:
      this->buttonGroup = 5;
      this->button = JOY_UP;
      break;
    case Btn5D:
      this->buttonGroup = 5;
      this->button = JOY_DOWN;
      break;
    case Btn6U:
      this->buttonGroup = 6;
      this->button = JOY_UP;
      break;
    case Btn6D:
      this->buttonGroup = 6;
      this->button = JOY_DOWN;
      break;
    case Btn7U:
      this->buttonGroup = 7;
      this->button = JOY_UP;
      break;
    case Btn7D:
      this->buttonGroup = 7;
      this->button = JOY_DOWN;
      break;
    case Btn7L:
      this->buttonGroup = 7;
      this->button = JOY_LEFT;
      break;
    case Btn7R:
      this->buttonGroup = 7;
      this->button = JOY_RIGHT;
      break;
    case Btn8U:
      this->buttonGroup = 8;
      this->button = JOY_UP;
      break;
    case Btn8D:
      this->buttonGroup = 8;
      this->button = JOY_DOWN;
      break;
    case Btn8L:
      this->buttonGroup = 8;
      this->button = JOY_LEFT;
      break;
    case Btn8R:
      this->buttonGroup = 8;
      this->button = JOY_RIGHT;
      break;
  }

  // Adds the JoystickButton instance to the event scheduler
  EventScheduler::getInstance()->addEventListener(this);
}

void JoystickButton::checkConditions() {
  // Keeps track of the button's current state
  bool currentButtonState = joystickGetDigital(joystickToCheck, buttonGroup, button);

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

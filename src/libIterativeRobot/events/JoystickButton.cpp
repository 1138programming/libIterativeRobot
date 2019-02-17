#include "libIterativeRobot/events/JoystickButton.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

JoystickButton::JoystickButton(pros::Controller* controller, pros::controller_digital_e_t button) {
  this->controller = controller;
  this->button = button;
}

bool JoystickButton::get() {
  return (controller->get_digital(button) == 1);
}

void JoystickButton::whenPressed(Command* command, Action action) {
  whenActivated(command, action);
}

void JoystickButton::whileHeld(Command* command, Action action) {
  whileActive(command, action);
}

void JoystickButton::whenReleased(Command* command, Action action) {
  whenDeactivated(command, action);
}

void JoystickButton::whileReleased(Command* command, Action action) {
  whileInactive(command, action);
}

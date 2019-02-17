#include "libIterativeRobot/events/JoystickChannel.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

JoystickChannel::JoystickChannel(pros::Controller* controller, pros::controller_analog_e_t channel) {
  this->controller = controller;
  this->channel = channel;
  EventScheduler::getInstance()->addEventListener(this);
}

void JoystickChannel::whenPassingThresholdForward(Command* command, Action action) {
  whenActivated(command, action);
}

void JoystickChannel::whilePastThreshold(Command* command, Action action) {
  whileActive(command, action);
}

void JoystickChannel::whenPassingThresholdReverse(Command* command, Action action) {
  whenDeactivated(command, action);
}

void JoystickChannel::whileWithinThreshold(Command* command, Action action) {
  whileInactive(command, action);
}

void JoystickChannel::setThreshold(std::int32_t threshold) {
  this->threshold = threshold;
}

bool JoystickChannel::get() {
  return (abs(controller->get_analog(channel)) > threshold);
}

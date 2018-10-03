#include "libIterativeRobot/events/JoystickChannel.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

JoystickChannel::JoystickChannel(pros::Controller* controller, pros::controller_analog_e_t channel) {
  this->controller = controller;
  this->channel = channel;
  EventScheduler::getInstance()->addEventListener(this);
}

void JoystickChannel::checkConditions() {
  if (pastThresholdCommand == NULL)
    return;

  int currentChannelState = controller->get_analog(channel);
  if (currentChannelState > threshold || currentChannelState < -threshold) {
    pastThresholdCommand->run();
  }
}

void JoystickChannel::whilePastThreshold(Command* pastThresholdCommand, std::int32_t threshold) {
  this->pastThresholdCommand = pastThresholdCommand;
  this->threshold = threshold;
}

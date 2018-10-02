#include "libIterativeRobot/events/JoystickChannel.h"
#include "libIterativeRobot/events/EventScheduler.h"

using namespace libIterativeRobot;

JoystickChannel::JoystickChannel(Joysticks joystick, Channels channel) {
  this->joystickToCheck = (int)joystick;
  this->channel = channel;
  EventScheduler::getInstance()->addEventListener(this);
}

void JoystickChannel::checkConditions() {
  if (pastThresholdCommand == NULL)
    return;

  int currentChannelState = joystickGetAnalog(joystickToCheck, channel);
  if (currentChannelState > threshold || currentChannelState < -threshold) {
    pastThresholdCommand->run();
  }
}

void JoystickChannel::whilePastThreshold(Command* pastThresholdCommand, int threshold) {
  this->pastThresholdCommand = pastThresholdCommand;
  this->threshold = threshold;
}

#include "RobotBase.h"
#include "pros/misc.hpp"
#include "events/EventScheduler.h"

using namespace libIterativeRobot;

RobotBase::RobotBase() {
  robotInit();
}

void RobotBase::robotInit() {
  printf("Robot created.\n");
}

void RobotBase::autonInit() {
  EventScheduler::getInstance()->clearCommandQueue();
  printf("Default autonInit() function\n");
}

void RobotBase::autonPeriodic() {
  printf("Default autonPeriodic() function\n");
  EventScheduler::getInstance()->update();
}

void RobotBase::teleopInit() {
  EventScheduler::getInstance()->clearCommandQueue();
  printf("Default teleopInit() function\n");
}

void RobotBase::teleopPeriodic() {
  printf("Default teleopPeriodic() function\n");
  EventScheduler::getInstance()->update();
}

void RobotBase::disabledInit() {
  EventScheduler::getInstance()->clearCommandQueue();
  printf("Default disabledInit() function\n");
}

void RobotBase::disabledPeriodic() {
  printf("Default disabledPeriodic() function");
}

void RobotBase::doOneCycle() {
  // This is my "tribute" of sorts to Squeak Smalltalk, the most
  // influential programming language to me and the first language
  // I've ever learned.
  // `doOneCycle` is the name of the method in PasteUpMorph that
  // ran the `step` methods for all of its submorphs and essentially
  // made the world tick. Similarly, this method is the method that
  // makes the robot tick.

  // For the v5, since we have a touchscreen and an ability to
  // have the user select which mode they may want, due to this,
  // we might take advantage of that and allow the user to manually
  // switch modes when they are not in a competition.
  if (pros::competition::is_disabled()) {
    // Robot is currently disabled
    if (lastState == RobotState::Disabled) {
      disabledPeriodic();
    } else {
      lastState = RobotState::Disabled;
      disabledInit();
    }
  } else {
    if (pros::competition::is_autonomous()) {
      // Robot is in autonomous mode
      if (lastState == RobotState::Auton) {
        autonPeriodic();
      } else {
        lastState = RobotState::Auton;
        autonInit();
      }
    } else {
      // Robot is in teleop
      if (lastState == RobotState::Teleop) {
        teleopPeriodic();
      } else {
        lastState = RobotState::Teleop;
        teleopInit();
      }
    }
  }
}

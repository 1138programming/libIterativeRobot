#include "RobotBase.h"
#include "pros/misc.hpp"
#include "events/EventScheduler.h"

using namespace libIterativeRobot;

RobotBase::RobotBase() {
}

void RobotBase::_privateRunRobot(void* param) {
    //RobotMain* robotInstance = RobotMain::getInstance();
    while (true) {
      doOneCycle();
    }
}

void RobotBase::doOneCycle() {
  // This is my "tribute" of sorts to Squeak Smalltalk, the most
  // influential programming language to me and the first language
  // I've ever learned.
  // `doOneCycle` is the name of the method in PasteUpMorph that
  // ran the `step` methods for all of its submorphs and essentially
  // made the world tick. Similarly, this method is the method that
  // makes the robot tick. - Edward Pedemonte

  // For the v5, since we have a touchscreen and an ability to
  // have the user select which mode they may want, due to this,
  // we might take advantage of that and allow the user to manually
  // switch modes when they are not in a competition.
  if (lastState == RobotState::None) {
    robotInit();
  }
  if (pros::competition::is_disabled()) {
    // Robot is currently disabled
    if (lastState == RobotState::Disabled) {
      EventScheduler::getInstance()->update();
      disabledPeriodic();
    } else {
      lastState = RobotState::Disabled;
      EventScheduler::getInstance()->initialize();
      disabledInit();
    }
  } else {
    if (pros::competition::is_autonomous()) {
      // Robot is in autonomous mode
      if (lastState == RobotState::Auton) {
        EventScheduler::getInstance()->update();
        autonPeriodic();
      } else {
        lastState = RobotState::Auton;
        EventScheduler::getInstance()->initialize(true);
        autonInit();
      }
    } else {
      // Robot is in teleop
      if (lastState == RobotState::Teleop) {
        EventScheduler::getInstance()->update();
        teleopPeriodic();
      } else {
        lastState = RobotState::Teleop;
        EventScheduler::getInstance()->initialize(true); // Add default commands too
        teleopInit();
      }
    }
  }
}

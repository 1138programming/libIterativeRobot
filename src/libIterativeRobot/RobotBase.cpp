#include "RobotBase.h"
#include "Robot.h"
#include "pros/misc.hpp"
#include "events/EventScheduler.h"

using namespace libIterativeRobot;

RobotBase::RobotBase() {
}

void RobotBase::_privateRunRobot(void* param) {
    RobotBase* robot = reinterpret_cast<RobotBase*>(param);
    std::uint32_t prev_time = pros::millis();
    const std::uint32_t delta = 10;
    while (true) {
      robot->doOneCycle();
      pros::Task::delay_until(&prev_time, delta);
    }
}

void RobotBase::runRobot() {
  // Just saying, if this doesn't work, try using the reinterepret cast on the method instead, instead of its pointer
  // reinterpret_cast<void (*)(void*)>(&_privateRunRobot<RobotMain>)
  pros::Task(
    reinterpret_cast<void (*)(void*)>(&_privateRunRobot),
    reinterpret_cast<void *>(this),
    TASK_PRIORITY_DEFAULT,
    TASK_STACK_DEPTH_DEFAULT,
    "libIterativeRobot Task"
  );
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
      disabledPeriodic();
    } else {
      lastState = RobotState::Disabled;
      libIterativeRobot::EventScheduler::getInstance()->initialize();
      disabledInit();
    }
  } else {
    if (pros::competition::is_autonomous()) {
      // Robot is in autonomous mode
      if (lastState == RobotState::Auton) {
        autonPeriodic();
        libIterativeRobot::EventScheduler::getInstance()->update();
      } else {
        lastState = RobotState::Auton;
        libIterativeRobot::EventScheduler::getInstance()->initialize();
        autonInit();
      }
    } else {
      // Robot is in teleop
      if (lastState == RobotState::Teleop) {
        teleopPeriodic();
        libIterativeRobot::EventScheduler::getInstance()->update();
      } else {
        lastState = RobotState::Teleop;
        libIterativeRobot::EventScheduler::getInstance()->initialize();
        teleopInit();
      }
    }
  }
}

void RobotBase::initializeRobot() {
  Robot::getInstance()->runRobot();
}

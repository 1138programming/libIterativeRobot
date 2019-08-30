#include "RobotBase.h"
#include "Robot.h"
#include "pros/misc.hpp"
#include "events/EventScheduler.h"

using namespace libIterativeRobot;

RobotBase::RobotBase() {
}

void RobotBase::printStuff() {
  printf("Private function called succesfully\n");
}

void RobotBase::_privateRunRobot(void* param) {
    //RobotMain* robotInstance = RobotMain::getInstance();
    printf("Run robot starting...\n");
    RobotBase* robot = reinterpret_cast<RobotBase*>(param);
    while (true) {
      printf("Loop is running\n");
      pros::delay(1000);
      robot->doOneCycle();
      //robot->printStuff();
    }
}

/*void my_task_fn(void* param) {
  while (true) {
    printf("Loop is running\n");
  }
}*/

void RobotBase::runRobot() {
  // Just saying, if this doesn't work, try using the reinterepret cast on the method instead, instead of its pointer
  // reinterpret_cast<void (*)(void*)>(&_privateRunRobot<RobotMain>)
  printf("Initializing task\n");
  pros::Task(
    //reinterpret_cast<void (*)(void*)>(&my_task_fn),
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
  printf("do one cycle\n");
  pros::delay(1000);
  if (lastState == RobotState::None) {
    robotInit();
  }
  if (pros::competition::is_disabled()) {
    printf("is disabled\n");
    pros::delay(1000);
    // Robot is currently disabled
    if (lastState == RobotState::Disabled) {
      //EventScheduler::getInstance()->update();
      disabledPeriodic();
    } else {
      lastState = RobotState::Disabled;
      //EventScheduler::getInstance()->initialize();
      disabledInit();
    }
  } else {
    if (pros::competition::is_autonomous()) {
      printf("is autonomous\n");
      pros::delay(1000);
      // Robot is in autonomous mode
      if (lastState == RobotState::Auton) {
        //EventScheduler::getInstance()->update();
        autonPeriodic();
      } else {
        lastState = RobotState::Auton;
        //EventScheduler::getInstance()->initialize(true);
        autonInit();
      }
    } else {
      printf("is teleop, state is %d\n", lastState);
      pros::delay(1000);
      // Robot is in teleop
      if (lastState == RobotState::Teleop) {
        //EventScheduler::getInstance()->update();
        printf("periodic\n");
        pros::delay(1000);
        teleopPeriodic();
      } else {
        lastState = RobotState::Teleop;
        //EventScheduler::getInstance()->initialize(true); // Add default commands too
        printf("init\n");
        pros::delay(1000);
        teleopInit();
      }
    }
  }
}

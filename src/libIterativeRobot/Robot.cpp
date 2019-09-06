#include "libIterativeRobot/Robot.h"
#include "events/EventScheduler.h"
//#include "events/JoystickButton.h"

Robot* Robot::instance = 0;
ExampleSubsystem* Robot::exampleSubsystem = 0;

Robot::Robot() {
}

void Robot::robotInit() {
}

void Robot::autonInit() {
  libIterativeRobot::EventScheduler::getInstance()->initialize();
}

void Robot::autonPeriodic() {
  libIterativeRobot::EventScheduler::getInstance()->update();
}

void Robot::teleopInit() {
  libIterativeRobot::EventScheduler::getInstance()->initialize();
}

void Robot::teleopPeriodic() {
  libIterativeRobot::EventScheduler::getInstance()->update();
}

void Robot::disabledInit() {
  libIterativeRobot::EventScheduler::getInstance()->initialize();
}

void Robot::disabledPeriodic() {
  //printf("Default disabledPeriodic() function\n");
}

Robot* Robot::getInstance() {
    if (instance == NULL) {
        instance = new Robot();
    }
    return instance;
}

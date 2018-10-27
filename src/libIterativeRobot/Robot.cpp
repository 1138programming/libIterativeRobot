#include "libIterativeRobot/Robot.h"
#include "events/EventScheduler.h"
//#include "events/JoystickButton.h"

Robot* Robot::instance = 0;
ExampleSubsystem* Robot::exampleSubsystem = 0;

Robot::Robot() {
  printf("Overridden robot constructor!\n");
  // Initialize any subsystems
  Robot::exampleSubsystem = new ExampleSubsystem();
}

void Robot::robotInit() {
  printf("Robot created.\n");
}

void Robot::autonInit() {
  printf("Default autonInit() function\n");
  libIterativeRobot::EventScheduler::getInstance()->clearCommandQueue();
  libIterativeRobot::EventScheduler::getInstance()->clearCommandGroupQueue();
}

void Robot::autonPeriodic() {
  // printf("Default autonPeriodic() function\n");
  libIterativeRobot::EventScheduler::getInstance()->update();
}

void Robot::teleopInit() {
  printf("Default teleopInit() function\n");
  libIterativeRobot::EventScheduler::getInstance()->clearCommandQueue();
  libIterativeRobot::EventScheduler::getInstance()->clearCommandGroupQueue();
}

void Robot::teleopPeriodic() {
  //printf("Default teleopPeriodic() function\n");
  libIterativeRobot::EventScheduler::getInstance()->update();
}

void Robot::disabledInit() {
  printf("Default disabledInit() function\n");
  libIterativeRobot::EventScheduler::getInstance()->clearCommandQueue();
  libIterativeRobot::EventScheduler::getInstance()->clearCommandGroupQueue();
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

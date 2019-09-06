#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "main.h"
#include "RobotBase.h"

class Robot : public libIterativeRobot::RobotBase {
  private:
    static Robot* instance;
    Robot();
  protected:
    void robotInit();
    void autonInit();
    void autonPeriodic();
    void teleopInit();
    void teleopPeriodic();
    void disabledInit();
    void disabledPeriodic();
  public:
    static Robot* getInstance();
};

#endif // _ROBOT_H_

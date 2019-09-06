#ifndef _ROBOTBASE_H_
#define _ROBOTBASE_H_

#include "main.h"
#include "pros/rtos.hpp"

namespace libIterativeRobot {
  class RobotBase {
    private:
      /**
       * @brief Possible robot states are None, Auton, Teleop, and Disabled
       */
      enum class RobotState {
        None,
        Auton,
        Teleop,
        Disabled,
      };

      /**
       * @brief Stores the last state of the robot
       */
      RobotState lastState = RobotState::None;

      /**
       * @brief
       */
      void doOneCycle();

      //void _privateRunRobot(void* param);
      //void my_task_fn(void* param);
      static void _privateRunRobot(void* param);
      void printStuff();
    protected:
      /**
        * @brief Runs when the robot starts up.
        */
      virtual void robotInit() = 0;

      /**
        * @brief Runs once each time the autonomous period begins.
        */
      virtual void autonInit() = 0;

      /**
        * @brief Runs in a loop during the autonomous period.
        */
      virtual void autonPeriodic() = 0;

      /**
        * @brief Runs once each time the teleoperated period begins.
        */
      virtual void teleopInit() = 0;

      /**
        * @brief Runs in a loop during the teleoperated period.
        */
      virtual void teleopPeriodic() = 0;

      /**
        * @brief Runs once each time the robot is disabled.
        */
      virtual void disabledInit() = 0;

      /**
        * @brief Runs in a loop while the robot is disabled.
        */
      virtual void disabledPeriodic() = 0;

      /**
        * Run the robot.
        *
        * This should be called with the main robot class as the template argument.
        * For example, if your robot is named `ExampleRobot`, you would call
        * it with `RobotBase::runRobot<ExampleRobot>()`
        */
      void runRobot();

      RobotBase();
    public:
      static void initializeRobot();
  };
}
#endif // _ROBOTBASE_H_

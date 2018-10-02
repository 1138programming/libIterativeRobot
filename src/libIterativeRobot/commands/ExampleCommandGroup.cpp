#include "libIterativeRobot/commands/ExampleCommandGroup.h"
#include "libIterativeRobot/commands/ExampleCommand.h"
//#include "libIterativeRobot/commands/ExampleWaitCommand.h"

using namespace libIterativeRobot;

ExampleCommandGroup::ExampleCommandGroup() {
//  addSequentialCommand(new ExampleWaitCommand());
  addSequentialCommand(new ExampleCommand());
}

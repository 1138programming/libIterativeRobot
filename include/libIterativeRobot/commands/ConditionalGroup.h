#ifndef _COMMANDS_CONDITIONALGROUP_H_
#define _COMMANDS_CONDITIONALGROUP_H_

#include "CommandGroup.h"
#include "LambdaGroup.h"

namespace libIterativeRobot {
  class ConditionalGroup : public CommandGroup {
    private:
      LambdaGroup* lambda;
      virtual void conditionalBody();
    protected:
      void addSequentialCommand(Command* aCommand, bool forget = false);
      void addParallelCommand(Command* aCommand, bool forget = false);
    public:
      ConditionalGroup();
      void run();
      void stop();
  };
}

#endif //_COMMANDS_CONDITIONAL_GROUP_H_

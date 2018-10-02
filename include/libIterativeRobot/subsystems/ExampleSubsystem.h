#ifndef _SUBSYSTEMS_EXAMPLESUBSYSTEM_H_
#define _SUBSYSTEMS_EXAMPLESUBSYSTEM_H_

#include "./Subsystem.h"

namespace libIterativeRobot {

class ExampleSubsystem : public Subsystem {
  public:
    void initDefaultCommand();
    ExampleSubsystem();
};

};

#endif // _SUBSYSTEMS_EXAMPLESUBSYSTEM_H_

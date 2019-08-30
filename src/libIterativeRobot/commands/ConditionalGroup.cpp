#include "./ConditionalGroup.h"

using namespace libIterativeRobot;

ConditionalGroup::ConditionalGroup() {
  lambda = new LambdaGroup();
}

void ConditionalGroup::addSequentialCommand(Command* aCommand, bool forget) {
  lambda->addSequentialCommand(aCommand, forget);
}

void ConditionalGroup::addParallelCommand(Command* aCommand, bool forget) {
  lambda->addParallelCommand(aCommand, forget);
}

void ConditionalGroup::run() {
  delete lambda;
  lambda = new LambdaGroup();
  conditionalBody();
  lambda->run();
}

void ConditionalGroup::stop() {
  lambda->stop();
}

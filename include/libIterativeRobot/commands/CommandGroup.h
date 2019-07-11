#ifndef _COMMANDS_COMMANDGROUP_H_
#define _COMMANDS_COMMANDGROUP_H_

#include "Command.h"
#include "main.h"
#include <vector>

namespace libIterativeRobot {

/**
 * A CommandGroup is a way to call commands in sequence or concurrently, or any combination of the two. It can be thought
 * of as consisting of a series of steps, with each step having one or more commands in it. All the commands in a step
 * are executed in parallel and the steps are executed in sequence. A command is added using the addSequentialCommand()
 * method, which adds a command into a new step. The addParallelCommand() method adds commands into the current last step.
 * A CommandGroup is also considered a command, so they can be added into other CommandGroups allowing for more complex
 * sequences of commands to be executed.
 *
 * CommandGroups carry all of the requirements of the commands in the currently executing step. This means that
 * CommandGroups' requirements change as they execute
 */

class CommandGroup : public Command {
  private:
    std::vector<std::vector<Command*>> commands; // 2d vector keeping track of all commands and command groups. Each vector in the 2d vector represents a sequential step. All the commands and command groups in each sequential step are run in parallel
    std::vector<std::vector<Subsystem*>> requirements; // 2d vector storing the requirements of each command and command group
    std::vector<std::vector<int>> added; // 2d vector keeping track of which commands and command groups have been added to the event scheduler
    std::vector<std::vector<int>> waitUntilFinished; // 2d vector keeping track of which commands and command groups the command group should wait for to finish
    size_t sequentialIndex = 0; // Keeps track of the current sequential step the command group is running
  public:
    std::vector<Subsystem*>& getRequirements(); // Returns a vector containing the subsystem requirements of all of the commands and command groups in the current sequential step

    bool canRun(); // Checks if all the commands and command groups in the command group can run
    void initialize(); // Set up the command for running
    void execute(); // Run the command
    bool isFinished(); // Returns whether or not the command group is finished. The execute function is called continuously until this is true or the command group is interrupted
    void end(); // Run when command group is finished
    void interrupted(); // Run when command group was interrupted

    void addSequentialCommand(Command* aCommand, bool waitUntilFinished = false); // Adds a command or command group to a new sequential step. Sequential steps are run in the order they are added
    void addParallelCommand(Command* aCommand, bool waitUntilFinished = false); // Adds a command or command group to be run in parallel with the commands and command groups in the current sequential steps

    void run(); // Runs this command group. May be called anywhere.
    void stop(); // Stops this command group while it is running. May be called anywhere.

    // ...and finally, the constructor!
    CommandGroup();
};

};

#endif // _COMMANDS_COMMANDGROUP_H_

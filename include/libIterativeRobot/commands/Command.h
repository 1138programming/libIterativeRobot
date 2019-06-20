#ifndef _COMMANDS_COMMAND_H_
#define _COMMANDS_COMMAND_H_

#include "main.h"
#include <vector>
#include "libIterativeRobot/subsystems/Subsystem.h"

namespace libIterativeRobot {

// Keeps track of the status of the command
enum Status {
  Idle = 0,
  Running,
  Finished,
  Interrupted
};

/**
 * The Command class is the base class for all commands.
 * Commands implement functionality for one or more subsystems,
 * and their execution and interactions are handled by the EventScheduler.
 * Commands are added to the EventScheduler
 * when their run() method is called and the command starts if its canRun() method returns true.
 * If its canRun() method returns false, the command does not start and is removed from the EventScheduler.
 * Once a command starts, its initialize() method is called once and then its execute()
 * method is called repeatedly. After each time the execute() method is called, the
 * command's isFinished() method is called and the command ends when it returns true.
 * After a command has finished, its end() method is called.
 *
 * Commands can also be removed from the EventScheduler by calling their stop() method. Calling the stop() method
 * will interrupt a command.
 *
 * When a command is interrupted, its interrupted() method is called and it is removed from the EventScheduler
 *
 * Subsystems that a command uses should be declared by calling the requires(...) method in its constructor.
 *
 * Every command has a priority which determines how it will interact with other commands.
 * If two commands use one or more of the same subsystems, the one with the higher priority will interrupt
 * the one with the lower priority if the lower priority command is already running, or prevent it from starting
 * if it has been added to the EventScheduler but has not yet started running.
 *
 * Default commands are special commands that have a priority of 0 and require only one subsystem.
 * Unlike regular commands, when they finish or are interrupted, they are not removed by the EventScheduler.
 * As a result, the EventScheduler continually attempts to run all default commands, and default commands
 * are constantly run while no other commands require the same subsystem. A command is made into a default
 * command
 */

class Command {
  private:
    std::vector<Subsystem*> subsystemRequirements; // Vector to keep track of which subsystems the command requires to run
  protected:
    void requires(Subsystem* aSubsystem); // Function to add a subsystem as one of a command's requirements
    friend class Subsystem;
  public:
    static const int DefaultCommandPriority = 0; // Priority of a default command is 0

    int priority = 1; // Commands can only be interrupted by commands with a higher priority
    Status status = Idle; // Keeps track of the status of the command

    std::vector<Subsystem*>& getRequirements(); // Returns the command's requirements as a vector pointer

    virtual bool canRun() = 0; // Whether or not the command can run right now. If false, it is ignored
    virtual void initialize() = 0; // Set up the command for running
    virtual void execute() = 0; // Run the command
    virtual bool isFinished() = 0; // Whether or not the command is finished. The execute() function is called continuously until this is true or the command is interrupted
    virtual void end() = 0; // Run when command is finished
    virtual void interrupted() = 0; // Run when command was interrupted by one with a higher priority

    void run(); // Runs this command. May be called anywhere.
    void stop(); // Stops this command while it is running. May be called anywhere.

    // ...and finally, the constructor!
    Command();
};

}; // namespace libIterativeRobot

#endif // _COMMANDS_COMMAND_H_

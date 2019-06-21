#ifndef _COMMANDS_COMMAND_H_
#define _COMMANDS_COMMAND_H_

#include "main.h"
#include <vector>
#include "libIterativeRobot/subsystems/Subsystem.h"

namespace libIterativeRobot {

/**
 * The Status of a command indicates what the EventScheduler should do with it. An Idle status means that the command
 * has not yet been initialized. Once initialized, a command's status is set to Running. After a command's isFinished
 * function returns true, the command's status is set to Finished. If a command is interrupted, its status is set to
 * Interrupted.
 */
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
 * Default commands are special commands that have a priority of 0 (the lowest possible priority) and require only
 * one subsystem. Unlike regular commands, when they finish or are interrupted, they are not removed by the EventScheduler.
 * As a result, the EventScheduler continually attempts to run all default commands, and default commands
 * are constantly run while no other commands require the same subsystem. A command is a default command
 * if it is passed to a subsystem's setDefaultCommand() method. The subsystem that the default command requires
 * is automatically added to its list of requirements, so it is not necessary to use the requires() method to add it.
 */

class Command {
  private:
    std::vector<Subsystem*> subsystemRequirements; /** Keeps track of which subsystems the command requires to run */
  protected:
    void requires(Subsystem* aSubsystem); /** Adds a subsystem as one of a command's requirements */
    Status status = Idle; /** Keeps track of the status of the command */

    friend class Subsystem;
    friend class EventScheduler;
  public:
    static const int DefaultCommandPriority = 0; /** The priority of a default command is 0 */

    int priority = 1; /** Higher priority commands interrupt lower priority commands */

    std::vector<Subsystem*>& getRequirements(); /** Returns the command's requirements as a vector pointer */

    virtual bool canRun() = 0; /** Returns whether or not the command can run right now. If false, the command is ignored */
    virtual void initialize() = 0; /** Sets up the command for execution. This method is called once before the command begins running */
    virtual void execute() = 0; /** Runs the command */
    virtual bool isFinished() = 0; /** Returns whether or not the command is finished. The execute() function is called continuously until this method returns true or the command is interrupted */
    virtual void end() = 0; /** Runs once when command is finished */
    virtual void interrupted() = 0; /** Runs once when command is interrupted */

    void run(); /** Adds the command to the EventScheduler */
    void stop(); /** Removes the command from the EventScheduler and interrupts it */

    // ...and finally, the constructor!
    Command(); /** Creates a new command */
};

}; // namespace libIterativeRobot

#endif // _COMMANDS_COMMAND_H_

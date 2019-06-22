#ifndef _COMMANDS_COMMAND_H_
#define _COMMANDS_COMMAND_H_

#include "main.h"
#include "libIterativeRobot/subsystems/Subsystem.h"
#include <vector>

namespace libIterativeRobot {

/**
 * @mainpage Refactored-Chainsaw documentation
 */

/**
 * Here is an embedded script:
 * @htmlonly
 * <p>Test!</p>
 * <script>
 *  alert("Script is working!");
 * </script>
 * @endhtmlonly
 */

/**
 * The Status of a command indicates what the EventScheduler should do with it. An Idle status means that the command
 * has not yet been initialized. Once initialized, a command's status is set to Running. After a command's isFinished
 * function returns true, the command's status is set to Finished. If a command is interrupted, its status is set to
 * Interrupted
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
 * when their run() method is called, and the command starts if its canRun() method returns true.
 * If its canRun() method returns false, the command does not start and is removed from the EventScheduler.
 * Once a command starts, its initialize() method is called once and then its execute()
 * method is called repeatedly. After each time the execute() method is called, the
 * command's isFinished() method is called. The command stops running when isFinished() returns true.
 * After a command has finished, its end() method is called.
 *
 * Commands can also be removed from the EventScheduler by calling their stop() method. Calling the stop() method
 * will interrupt a command.
 *
 * When a command is interrupted, its interrupted() method is called and it is removed from the EventScheduler
 *
 * Subsystems that a command uses should be declared by calling the requires() method in its constructor.
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
    /**
     * Keeps track of which subsystems the command requires to run
     *
     * @htmlonly
     * <script>
     * var rows = document.querySelectorAll(".memItemRight");
     * for (var i = 0; i < rows.length; i++) {
	   *   let index = rows[i].innerHTML.indexOf("=0");
     *   if (index !== -1)
	   *     rows[i].innerHTML = rows[i].innerHTML.slice(0, index) + " = 0";
     * }
     * </script>
     * @endhtmlonly
     */
    std::vector<Subsystem*> subsystemRequirements;

    /**
     * Higher priority commands interrupt lower priority commands
     */
    int priority = 1;
  protected:
    /**
     * @brief Adds a subsystem as one of a command's requirements
     * @param aSubsystem The subsystem that the command requires
     */
    void requires(Subsystem* aSubsystem);

    /**
     * @brief Keeps track of the status of the command
     */
    Status status = Idle;

    /**
     * @brief Gets the requirements that a command uses
     *
     * Used by the EventScheduler to decide whether the command can run.
     * @return The command's requirements as a vector pointer
     */
    std::vector<Subsystem*>& getRequirements();

    friend class Subsystem;
    friend class EventScheduler;
  public:
    /**
     * @brief The priority of a default command is 0.
     */
    static const int DefaultCommandPriority = 0;

    /**
     * @brief Whether the command can run or not
     *
     * Called by the EventScheduler before a command starts running to check whether it can run or not
     * @return Whether or not the command can run
     */
    virtual bool canRun() = 0;

    /**
     * @brief Called once before the command runs
     *
     * Code needed to sets up the command for execution can be put here.
     * This method is called once before the command begins running
     */
    virtual void initialize() = 0;

    /**
     * @brief Runs the command
     */
    virtual void execute() = 0;

    /**
     * @brief Called by the EventScheduler while the command is running to check if it is finished
     * @return Whether or not the command is finished
     */
    virtual bool isFinished() = 0;

    /**
     * @brief Runs once when command is finished.
     */
    virtual void end() = 0;

    /**
     * @brief Runs once when a command is interrupted.
     */
    virtual void interrupted() = 0;

    /**
     * @brief Adds the command to the EventScheduler.
     */
    void run();

    /**
     * @brief Removes the command from the EventScheduler and interrupts it.
     */
    void stop();

    /**
     * @brief Creates a new command.
     * @return A command
     */
    Command();
};

}; // namespace libIterativeRobot

#endif // _COMMANDS_COMMAND_H_

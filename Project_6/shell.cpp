//  shell.cpp
//  CS232 Command Shell
//
//  Created by Ben DeWeerd on 4.10.2022
//
//  The Shell class creates an interactive terminal shell to help the user
//  run commands and programs. Handles builtin commands (cd, pwd, exit)
//  and searches the system path for other programs.

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"

// #define DEBUGME 1

Shell::Shell()
{
    prompt = Prompt();
    path = Path();
}

string Shell::colorize(string in, Color color)
{
    // add color to outputs for better readability
    // see https://www.tutorialspoint.com/how-to-output-colored-text-to-a-linux-terminal
    switch (color)
    {
    case red:
        return "\033[1;31m" + in + "\033[0m";
        break;
    case green:
        return "\033[1;32m" + in + "\033[0m";
        break;
    case blue:
        return "\033[1;34m" + in + "\033[0m";
        break;
    default:
        return in;
    }
}

void Shell::run()
{

#if DEBUGME
    path.print();
#endif

    // forever loop: continue to prompt user until exit
    while (true)
    {
        prompt.set();
        cout << colorize(prompt.get(), green);

        CommandLine commandLine = CommandLine(cin);

        argv = commandLine.getArgVector();
        argc = commandLine.getArgCount();

        // if there aren't any arguments, loop again
        if (!argc)
            continue;

        string command = argv[0];

        // check for builtin commands
        if (command == "exit")
        {
            exit(0);
        }
        else if (command == "cd")
        {
            cd(argv[1]);
        }
        else if (command == "pwd")
        {
            pwd();
        }
        else
        {
            // external command
            int pathIndex = path.find(argv[0]);

            if (pathIndex >= 0)
            {
                // determine if we need to run in background or not
                noAmpersand = commandLine.noAmpersand();

                // fork a new process
                // see https://stackoverflow.com/questions/36754510/understanding-fork-exec-and-wait-in-c-linux
                // pid = -1 -> fork() failure
                // pid = 0 -> child process
                // pid > 0 -> parent process
                pid_t pid = fork();

                if (pid < 0)
                {
                    // fork() failure - no child process created
                    cout << colorize("Shell: fork() error", red) << endl;
                    sched_yield();
                    continue;
                }
                else if (pid == 0)
                {
                    // child process - execute new program
                    string commandPath = path.getDirectory(pathIndex) + "/" + argv[0];
#if DEBUGME
                    cout << colorize("Shell: command path: ", blue) << commandPath << endl;
#endif
                    execve(commandPath.c_str(), argv, NULL);
                    // execve only returns on failure
                    cout << colorize("Shell: execve() error", red) << endl;
                    exit(EXIT_FAILURE);
                }
                else
                {
                    // parent process - wait for child if no ampersand
                    if (noAmpersand)
                    {
                        int childStatus;
                        waitpid(pid, &childStatus, 0);
                    }
                    else
                    {
                        sched_yield();
                        continue;
                    }
                }

#if DEBUGME
                cout << colorize("Shell: Program " + command + " found in directory " + path.getDirectory(pathIndex), blue) << endl;
#endif
            }
            else
            {
                cout << colorize("command " + command + " not found.", red) << endl;
            }
        }
    }
}

void Shell::cd(const string dir)
{
    try
    {
        int success = chdir(dir.c_str());
        if (success != 0)
            throw 1;
    }
    catch (...)
    {
        // unable to move to specified path
        cout << colorize("invalid path.", red) << endl;
    }
}

void Shell::pwd()
{
    string cwd = getcwd(NULL, 0);
    cout << cwd << endl;
}

Shell::~Shell()
{
    delete &path;
    delete &prompt;
}
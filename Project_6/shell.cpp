//
// shell.cpp
// CS232 Command Shell
//
// Created by Ben DeWeerd on 3.28.2022
//

#include <string>
#include <unistd.h>
#include <sys/types.h> // waitpid()
#include <sys/wait.h>  // waitpid()
#include "shell.h"

#define DEBUGME 1

Shell::Shell()
{
    prompt = Prompt();
    path = Path();
}

void Shell::run()
{

#if DEBUGME
    path.print();
#endif

    while (true)
    {
        prompt.set();
        cout << color(prompt.get(), 32);

        CommandLine commandLine = CommandLine(cin);

        argv = commandLine.getArgVector();
        argc = commandLine.getArgCount();

        // if there aren't any arguments, loop again
        if (!argc)
            continue;

        string command = argv[0];

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
            // TODO: search path for commands that aren't built-in
            // https://stackoverflow.com/questions/36754510/understanding-fork-exec-and-wait-in-c-linux
            int pathIndex = path.find(argv[0]);

            // pid = -1 -> fork() failure
            // pid = 0 -> child process
            // pid > 0 -> parent process
            if (pathIndex >= 0)
            {
                // determine if we need to run in background or not
                noAmpersand = commandLine.noAmpersand();
                // fork a new process
                pid_t pid = fork();
                if (pid < 0)
                {
                    // fork() failure, no child process created
                    cout << color("Shell: fork() error", 31) << endl;
                    sched_yield();
                    continue;
                }
                else if (pid == 0)
                {
                    // child process - execute new program
                    string commandPath = path.getDirectory(pathIndex) + "/" + argv[0];
                    // TODO: handle ampersands (running in background)
                    cout << color("Shell: command path: ", 34) << commandPath << endl;
                    execve(commandPath.c_str(), argv, NULL);
                    perror("execve"); // execve only returns on an error
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
                // the program exists in the path
                string pathName = path.getDirectory(pathIndex);
                cout << color("Shell: Program " + command + " found in directory " + pathName, 34) << endl;
            }
            else
            {
                cout << color("Shell: Command " + command + " not found.", 31) << endl;
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
        cout << color("Invalid destination path.", 31) << endl;
    }
}

void Shell::pwd()
{
    string cwd = getcwd(NULL, 0);
    cout << cwd << endl;
}

string Shell::color(string in, int color){
    /*
    Color codes:
        31 - red
        32 - green
        34 - blue
    */

    switch(color){
        case 31:    // red 
            return "\033[1;31m" + in + "\033[0m";
            break;
        case 32:    // green
            return "\033[1;32m" + in + "\033[0m";
            break;
        case 34:
            return "\033[1;34m" + in + "\033[0m";
            break;
        default:
            return in;
    }
}

Shell::~Shell()
{
    // destructor
}
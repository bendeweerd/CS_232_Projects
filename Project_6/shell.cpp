//
// shell.cpp
// CS232 Command Shell
//
// Created by Ben DeWeerd on 3.28.2022
//

#include <string>
#include <unistd.h>
#include "shell.h"

// #define DEBUGME 1

Shell::Shell()
{
    prompt = Prompt();
    path = Path();
}

void Shell::run()
{

#if DEBUGME
    path.print();
    path.find("cowthink");
    path.find("code");
    path.find("eclipse");
    path.find("mahjonng");
    path.find("subl");
    path.find("discord");
#endif

    while(true)
    {
        prompt.set();
        cout << prompt.get();
        
        CommandLine commandLine = CommandLine(cin);
        
        argv = commandLine.getArgVector();
        argc = commandLine.getArgCount();

        // if there aren't any arguments, loop again
        if(!argc) continue;

        string command = argv[0];

        if(command == "exit") {
            exit(0);
        } else if (command == "cd") {
            cd(argv[1]);
        } else if (command == "pwd") {
            pwd();
        } else {
            // TODO: search path for commands that aren't built-in
            int pathIndex;
            if(pathIndex = path.find(command) >= 0){
                // the program exists in the path 
                string pathName = path.getDirectory(pathIndex);
                cout << "Program " << command << " found in directory " << pathName << endl;
            } else {
                cout << "Program " << command << " not found." << endl;
            }
        }
    }
}

void Shell::cd(string dir)
{
    try{
        int success = chdir(dir.c_str());
        if(success != 0) throw 1;
    }
    catch(...){
        cout << "Invalid destination path." << endl;
    }
}

void Shell::pwd()
{
    string cwd = getcwd(NULL, 0);
    cout << cwd << endl;
}

Shell::~Shell()
{
    // destructor
}
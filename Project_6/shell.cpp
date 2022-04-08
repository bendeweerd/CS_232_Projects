//
// shell.cpp
// CS232 Command Shell
//
// Created by Ben DeWeerd on 3.28.2022
//

#include <string>
#include <unistd.h>
#include "shell.h"

Shell::Shell()
{
    prompt = Prompt();
}

void Shell::run()
{
    while(true)
    {
        prompt.set();
        cout << prompt.get();
        
        CommandLine commandLine = CommandLine(cin);
        
        argv = commandLine.getArgVector();
        argc = commandLine.getArgCount();

        string command;
        command = argv[0];

        if(command == "exit") {
            exit(0);
        } else if (command == "cd") {
            cd(argv[1]);
        } else if (command == "pwd") {
            pwd();
        } else {
            // TODO: search path for commands that aren't built-in
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
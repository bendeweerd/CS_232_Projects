//  shell.h
//  CS232 Command Shell
//
//  Created by Ben DeWeerd on 4.10.2022
//
//  Header file for the Shell class.

#ifndef __CS232_Command_Shell__shell__
#define __CS232_Command_Shell__shell__

#include <iostream>
#include <string>
#include "prompt.h"
#include "commandline.h"
#include "path.h"
using namespace std;

enum Color { red, green, blue };

class Shell
{
public:
    Shell();
    string colorize(string in, Color color);
    void run();
    void cd(const string dir);
    void pwd();
    ~Shell();
private:
    Prompt prompt;
    Path path;
    char **argv;
    int argc;
    bool noAmpersand;
};

#endif /* defined(__CS232_Command_Shell__shell__) */

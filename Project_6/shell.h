//
//  shell.h
//  CS232 Command Shell
//
//  Created by Ben DeWeerd on 3/28/2022.
//

#ifndef __CS232_Command_Shell__shell__
#define __CS232_Command_Shell__shell__

#include <iostream>
#include <string>
#include "prompt.h"
#include "commandline.h"
#include "path.h"
using namespace std;

class Shell
{
public:
    Shell();
    void run();
    void cd(string dir);
    void pwd();
    ~Shell();

private:
    Prompt prompt;
    Path path;
    char **argv;
    int argc;
};

#endif /* defined(__CS232_Command_Shell__shell__) */

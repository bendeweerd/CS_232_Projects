//  path.cpp
//  CS232 Command Shell
//
//  Created by Ben DeWeerd on 4.10.2022
//
//  The Path class parses the system path variable into a list of
//  directories in which to search for programs. It can also search the
//  list of directories for a specified program name.

#include "path.h"
#include <stdlib.h>
#include <iostream>
#include <sys/types.h> // directories
#include <dirent.h>    // directories

// #define DEBUGME 1

Path::Path()
{
    // parse program directories from the system PATH variable
    // see https://stackoverflowx.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    string env = getenv("PATH");
    string delimiter = ":";

    size_t pos = 0;
    string currentPath;
    while ((pos = env.find(delimiter)) != string::npos)
    {
        currentPath = env.substr(0, pos);
        path.push_back(currentPath);
        env.erase(0, pos + 1);
    }
}

int Path::find(const string &program) const
{
    // look for the program in each directory included in the path
    for (int i = 0; i < path.size(); i++)
    {
        DIR *dir;
        dirent *ent;
        dir = opendir(path[i].c_str());
        if (dir)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_name == program)
                {
#if DEBUGME
                    cout << "Path: program " << ent->d_name << " found in " << path[i] << endl;
#endif
                    // program located in this directory - return its index and close it
                    closedir(dir);
                    return i;
                }
            }
        }
        closedir(dir);
    }
#if DEBUGME
    cout << "Path: program " << program << " not found" << endl;
#endif
    // failed to locate program
    return -1;
}

string Path::getDirectory(int i) const
{
    return path[i];
}

void Path::print()
{
    // print each directory and its index
    cout << "\n************ Path **********" << endl;
    for (int i = 0; i < path.size(); i++)
    {
        cout << i << ": " << path[i] << endl;
    }
    cout << "****************************\n" << endl;
}

Path::~Path()
{
}
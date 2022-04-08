//
// path.cpp
// CS232 Command Shell
//
// Created by Ben DeWeerd on 4.08.2022
//

#include "path.h"
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>  // directories
#include <dirent.h>     // directories

// #define DEBUGME 1

Path::Path()
{
    //https://stackoverflowx.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    string env = getenv("PATH");
    string delimiter = ":";

    size_t pos = 0;
    string currentPath;
    while((pos = env.find(delimiter)) != string::npos){
        currentPath = env.substr(0, pos);
        path.push_back(currentPath);
        env.erase(0, pos + delimiter.length());
    }
}

int Path::find(const string& program) const 
{
    int result = -1;
    for(int i = 0; i < path.size(); i++){
        DIR *dir;
        struct dirent *ent;
        dir = opendir(path[i].c_str());
        if(dir){
            while((ent = readdir(dir)) != NULL){
                if(ent->d_name == program){
                    result = i;
#if DEBUGME
                    cout << "program " << ent->d_name << " found in " << path[i] << endl;
#endif
                    return result;
                }
            }
            closedir(dir);
        }
    }
#if DEBUGME
    cout << "program " << program << " not found" << endl;
#endif
    return result;
}

string Path::getDirectory(int i) const
{
    return path[i];
}

void Path::print()
{
    cout << "\n************ Path **********" << endl;
    for (int i = 0; i < path.size(); i++){
        cout << path[i] << endl;
    }
    cout << "****************************\n" << endl;
}

Path::~Path()
{
    // destructor
}
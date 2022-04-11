//  path.h
//  CS232 Command Shell
//
//  Created by Ben DeWeerd on 4.10.2022
//
//  Header file for the Path class.

#ifndef __CS232_Command_Shell__path__
#define __CS232_Command_Shell__path__

#include <string>
#include <vector>

using namespace std;

class Path
{
public:
    Path();
    int find(const string &program) const;
    string getDirectory(int i) const;
    void print();
    ~Path();

private:
    vector<string> path;
};

#endif /* defined(__CS232_Command_Shell__path__) */

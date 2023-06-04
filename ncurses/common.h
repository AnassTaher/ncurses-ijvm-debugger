#pragma once

#include <iostream>
#include <ncurses.h>
#include <string>
#include <cctype>
#include <map>
#include <set>
#include "ijvm.h"

struct Instruction{
    int args;
    std::string name;
};

extern std::map<int,Instruction> lookup;

void printProgram();
void printStep();




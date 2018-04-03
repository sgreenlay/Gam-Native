#pragma once

#include "Util.h"

struct rule
{
    int start;
    int end;
    double probability;
};

vector<rule> parse(const char* str);

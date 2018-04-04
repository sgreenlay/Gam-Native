#pragma once

#include "Util.h"

// 1 -50%-> 2
// 1 -50%-> 2,1

struct rule
{
    int start = -1;
    double probability = 0.0;
    int end = -1;
    int branch = -1;
};

vector<rule> parse(const char* str);

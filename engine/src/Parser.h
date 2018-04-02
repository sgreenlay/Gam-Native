#pragma once

#include "Util.h"

enum class rule_kind
{
    grow_forward,
    photosynth,
    grow_sideways,
};

struct rule
{
    rule_kind kind;

    double min_sugar_limit;

    int grow_cell_type() const { return data; }
    int new_state() const { return state_change; }

private:
    int data;
    int state_change;
};

struct ruleset
{
    vector<rule> rules;
};

vector<ruleset> parse(const char* str, int len);

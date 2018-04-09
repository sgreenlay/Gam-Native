
#include "Util.h"

struct rule;

struct vec2
{
    double x;
    double y;
};

inline vec2 operator+(vec2 l, vec2 r) { return vec2{ l.x + r.x, l.y + r.y }; }
inline vec2 operator-(vec2 l, vec2 r) { return vec2{ l.x - r.x, l.y - r.y }; }
inline vec2 operator*(vec2 v, double s) { return vec2{ v.x*s, v.y*s }; }
inline vec2 operator*(double s, vec2 v) { return vec2{ v.x*s, v.y*s }; }
inline vec2 operator/(vec2 v, double s) { return vec2{ v.x / s, v.y / s }; }

inline vec2& operator+=(vec2& l, vec2 r) { return l = l + r; }
inline vec2& operator-=(vec2& l, vec2 r) { return l = l - r; }
inline vec2& operator*=(vec2& l, double r) { return l = l * r; }
inline vec2& operator/=(vec2& l, double r) { return l = l / r; }

inline vec2 vec2_from_angle(double rads) { return vec2{ cos(rads), sin(rads) }; }
inline double dotproduct(vec2 a, vec2 b) { return a.x*b.x + a.y*b.y; }
inline double norm2(vec2 a) { return a.x*a.x + a.y*a.y; }

struct branch
{
    double width;
    double height;

    vec2 pos;
    double r;

    vec2 vel;
    double dr;

    int state;

    int parent;
};

struct constraint
{
    int b1;
    int b2;
    double r;
};

struct plant
{
    static plant make_seed(vec2 start, double direction);
    static plant make_random(vec2 start, double direction);
    static void make_branch(vec2 v, double r, plant& ret, int last);

    vector<branch> branches;
    vector<constraint> constraints;

    void physics_into(plant& out, vec2 gravity, double floor) const;
    void grow_into(plant& out, span<rule> rules) const;

    void render();
};

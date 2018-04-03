
#include "Util.h"

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

inline vec2 vec2_from_angle(double rads) { return vec2{ cos(rads), sin(rads) }; }

struct branch
{
    double width;
    double height;

    vec2 pos;
    double r;

    vec2 vel;
    double dr;
};

struct constraint
{
    int b1;
    int b2;
    double r;
};

struct plant
{
    static plant make_random(double x, double y);
    static void make_branch(vec2 v, double r, plant& ret);

    vector<branch> branches;
    vector<constraint> constraints;

    void physics_into(plant& out) const;

    void render()
    {
        for (auto& b : branches)
        {
            drawRect(
                (int)b.pos.x,
                (int)b.pos.y,
                (int)b.width,
                (int)b.height,
                0, 255, 0, 1.0f,
                b.r);
        }
    }
};

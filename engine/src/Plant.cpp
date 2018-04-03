#include "Plant.h"

plant plant::make_random(double x, double y)
{
    vec2 v{ x, y };

    plant ret;

    double r = PI/2;

    int last = -1;

    // 0 -50%-> (1, grow #0 forward)
    // 0 -30%-> (1, grow #0 forward; grow #2 +90 degrees)
    // 0 -20%-> (1, nothing)
    // 1 -> (1, nothing)
    // 2 -50%-> (1, grow #2 forward)
    // 2 -50%-> (1, nothing)

    int i = 0;
    do
    {
        ret.branches.emplace_back();
        auto& b = ret.branches.back();
        b.width = 25;
        b.height = 5;
        b.pos = v;
        b.vel = vec2{ 0,0 };
        b.r = r;
        b.dr = 0;

        if (last > -1)
            ret.constraints.emplace_back(constraint{last, ret.branches.size() - 1, 0});
        last = ret.branches.size()-1;

        v = v + 25 * vec2_from_angle(r);
        r += random() * 0.1;

        if (random() < 0.5)
        {
            make_branch(v, r - PI / 2, ret);
        }
        if (random() < 0.5)
        {
            make_branch(v, r + PI / 2, ret);
        }
    } while (random() + i*0.05 - 0.2 < 0.95 && i++ != 10);

    return ret;
}

void plant::make_branch(vec2 v, double r, plant& ret)
{
    int i = 0;
    do
    {
        ret.branches.emplace_back();
        //ret.constraints.emplace_back(constraint{ ret.branches.size() - 2, ret.branches.size() - 1, 0 });
        auto& b = ret.branches.back();
        b.width = 15;
        b.height = 2;
        b.pos = v;
        b.r = r;
        b.vel = vec2{ 0,0 };
        b.dr = 0;

        v = v + 15 * vec2_from_angle(r);
        r += random() * 0.5;
    } while (random()  + i*0.1 - 0.2 < 0.8 && i++ != 10);
}

void plant::physics_into(plant& out) const
{
    out.branches.resize(branches.size());
    out.constraints.resize(constraints.size());

    for (int x = 0; x < branches.size(); ++x)
    {
        auto& tgt = out.branches[x];
        auto& src = branches[x];
        tgt = src;
        tgt.pos = tgt.pos + src.vel;
        tgt.r += src.dr;
        tgt.vel = tgt.vel * 0.99;
        tgt.dr *= 0.99;
    }

    for (int y = 0; y < constraints.size(); ++y)
    {
        auto& c = constraints[y];
        out.constraints[y] = c;
        auto& b1src = branches[c.b1];
        auto& b2src = branches[c.b2];
        auto& b1tgt = out.branches[c.b1];
        auto& b2tgt = out.branches[c.b2];

        auto d = b1src.pos + b1src.width * vec2_from_angle(b1src.r) - b2src.pos;
        d = d * 0.01;
        b2tgt.vel = b2tgt.vel + d;
        b1tgt.vel = b1tgt.vel - d;
    }

    for (auto&& b : out.branches)
    {
        if (b.pos.y < 30) b.vel.y = 0;
        else
            b.vel.y -= 0.01;
    }
}
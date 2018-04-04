#include "Plant.h"

plant plant::make_random(vec2 start, double direction)
{
    vec2 v = start;
    double r = direction;

    plant ret;

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

        v = v + b.width * vec2_from_angle(r);
        r += random() * 0.1;

        int cur_index = ret.branches.size() - 1;

        if (random() < 0.5)
        {
            make_branch(v, r - PI / 2, ret, cur_index);
        }
        if (random() < 0.5)
        {
            make_branch(v, r + PI / 2, ret, cur_index);
        }
    } while (random() + i*0.05 - 0.2 < 0.95 && i++ != 10);

    return ret;
}

void plant::make_branch(vec2 v, double r, plant& ret, int last)
{
    int i = 0;
    do
    {
        ret.branches.emplace_back();
        ret.constraints.emplace_back(constraint{ last, ret.branches.size() - 1, 0 });
        auto& b = ret.branches.back();
        b.width = 15;
        b.height = 2;
        b.pos = v;
        b.r = r;
        b.vel = vec2{ 0,0 };
        b.dr = 0;

        v = v + 15 * vec2_from_angle(r);
        r += random() * 0.5;

        last = ret.branches.size() - 1;
    } while (random()  + i*0.1 - 0.2 < 0.8 && i++ != 10);
}

void plant::physics_into(plant& out, vec2 gravity, double floor) const
{
    auto norm2g = norm2(gravity);

    out.branches.resize(branches.size());
    out.constraints.resize(constraints.size());

    for (int x = 0; x < branches.size(); ++x)
    {
        auto& tgt = out.branches[x];
        auto& src = branches[x];
        tgt = src;
        tgt.pos += src.vel;
        tgt.r += src.dr;
        tgt.vel *= 0.99; // air friction
        tgt.dr *= 0.99; // air friction

        //gravity
        tgt.pos += gravity;
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
        d *= 0.2;
        b2tgt.pos += d;
        b1tgt.pos -= d;
    }

    for (auto&& b : out.branches)
    {
        if (b.pos.y > floor - 30) {
            b.pos.y = floor - 30;
            b.vel.y = 0;
        }
    }

    // gravity
    for (int x = 0; x < branches.size(); ++x)
    {
        auto& src = branches[x];
        auto& tgt = out.branches[x];

        tgt.vel += gravity;
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
        d *= 0.1;
        b2tgt.vel += d;
        b1tgt.vel -= d;
    }
}
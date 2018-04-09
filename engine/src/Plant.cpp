#include "Plant.h"

#include "Parser.h"

plant plant::make_seed(vec2 v, double r)
{
    plant ret;
    ret.branches.emplace_back();
    auto& b = ret.branches.back();
    b.width = 100;
    b.height = 5;
    b.pos = v;
    b.vel = {};
    b.r = r;
    b.dr = {};
    b.state = 0;
    b.parent = -1;

    return ret;
}

plant plant::make_random(vec2 start, double direction)
{
    vec2 v = start;
    double r = direction;

    plant ret;

    int last = -1;

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
        b.state = 0;
        b.parent = last;

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
        b.state = 0;
        b.parent = last;

        v = v + 15 * vec2_from_angle(r);
        r += random() * 0.5;

        last = ret.branches.size() - 1;
    } while (random()  + i*0.1 - 0.2 < 0.8 && i++ != 10);
}

void plant::physics_into(plant& out, vec2 gravity, double floor) const
{
#if defined(ENABLE_PHYSICS)
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
        tgt.vel *= 0.95; // air friction
        tgt.dr *= 0.95; // air friction

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
        d *= 0.2;
        b2tgt.vel += d;
        b1tgt.vel -= d;
    }
#endif
}

void plant::grow_into(plant& out, span<rule> rules) const
{
    out.branches.resize(branches.size());
    out.constraints.resize(constraints.size());

    vector<branch> tmp;

    for (int x = 0; x < branches.size(); ++x)
    {
        auto& tgt = out.branches[x];
        auto& src = branches[x];
        tgt = src;

        auto r = random();

        for (auto&& rule : rules)
        {
            if (rule.start == src.state)
            {
                if (rule.probability < 0 || rule.probability > 1)
                {
                    console_log("invalid probability ", rule.probability * 100);
                    continue;
                }
                if (r <= rule.probability)
                {
                    // activate rule
                    tgt.state = rule.self;
                    if (rule.branch != -1)
                    {
                        tmp.emplace_back(src);
                        tmp.back().parent = x;
                        tgt.dr = (tgt.dr - 0.01) / 2;
                        tmp.back().dr = tgt.dr + 0.01;
                    }
                    if (rule.forward != -1)
                    {
                        // constraints transfer, so we actually move "this" to the tmp vector with a constraint back.
                        tgt.parent = branches.size() + tmp.size();
                        tgt.width /= 1.8;
                        tmp.emplace_back(src);
                        tmp.back().width /= 1.8;

                        tmp.back().dr = tgt.dr;
                        tgt.dr = 0;
                    }
                    break;
                }
                else
                {
                    r = (r - rule.probability) / (1 - rule.probability);
                }
            }
        }
    }

    for (auto&& t : tmp)
        out.branches.emplace_back(t);

    // Robert, this is terrible...
    struct
    {
        vector<branch>& branches;
        vector<int> visiting;

        void operator()() {
            visiting.resize(branches.size(), 0);
            for (int x = 0; x < branches.size(); ++x) (*this)(x);
        }

        vec2 operator()(int i) {
            if (visiting[i] == 1)
            {
                abort("cycle in graph");
                return vec2{ 0 };
            }
            if (visiting[i] == 0)
            {
                visiting[i] = 1;
                auto p = branches[i].parent;
                if (p != -1) {
                    branches[i].pos = (*this)(p);
                    branches[i].r = branches[branches[i].parent].r + branches[i].width * branches[i].dr;
                }
                visiting[i] = 2;
            }
            return branches[i].pos + branches[i].width * vec2_from_angle(branches[i].r);
        }
    } p{out.branches};
    p();
}

void plant::render()
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
#include "Parser.h"

enum token_type
{
    error,
    number,     // [0-9]
    transition, // -[0-99]->
    split,      // ;
    end
};

struct token
{
    token_type type;
    int value;
};

struct parser
{
    parser(const char* str) : str(str) {}

    optional<rule> parse_rule()
    {
        token a = get_next();
        if (a.type != number)
        {
            return nullopt;
        }

        token p = get_next();
        if (p.type != transition)
        {
            return nullopt;
        }

        token b = get_next();
        if (b.type != number)
        {
            return nullopt;
        }

        return rule{ a.value,  b.value, p.value / 100.0 };
    }

private:

    optional<int> get_number()
    {
        char c = *str;

        if (c < '0' || c > '9')
        {
            return nullopt;
        }

        int value = 0;
        while (c >= '0' && c <= '9')
        {
            int digit = c-'0';
            value = value * 10 + digit;

            c = *++str;
        }

        return value;
    }

    token get_next()
    {
        if (*str == '\0') return { end };

        auto n = get_number();
        if (n.has_value()) return { number, n.value() };

        char c = *str++;

        if (c == '-')
        {
            auto p = get_number();
            if (!p.has_value())
            {
                return { error };
            }
            if (*str++ != '%')
            {
                return { error };
            }
            if (*str++ != '-')
            {
                return { error };
            }
            if (*str++ != '>')
            {
                return { error };
            }
            return { transition, p.value() };
        }

        if (c == ';') return { split };

        return { error };
    }

    const char* str;
};

vector<rule> parse(const char* str)
{
    vector<rule> ret;

    parser p(str);

    auto r = p.parse_rule();
    while (r.has_value())
    {
        ret.emplace_back(r.value());
        r = p.parse_rule();
    }

    return ret;
}
